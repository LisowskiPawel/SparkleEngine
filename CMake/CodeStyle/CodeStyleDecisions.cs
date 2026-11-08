using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;

public static class CodeStyleDecisions
{
    private static readonly Regex LexicalContents = new Regex(
        "(?m)^[ \\t]*#(?:[^\\n]*\\\\\\n)*[^\\n]*|" +
        "(?:u8|u|U|L)?R\"(?<delimiter>[^ ()\\\\\\t\\r\\n]{0,16})\\([\\s\\S]*?\\)\\k<delimiter>\"|" +
        "(?:u8|u|U|L)?\"(?:\\\\[\\s\\S]|[^\"\\\\])*\"|" +
        "(?<![\\w])(?:u8|u|U|L)?'(?:\\\\[^\\n]|[^'\\\\\\n])*'|//[^\\n]*|/\\*[\\s\\S]*?\\*/",
        RegexOptions.Compiled);

    private static readonly Regex IfHead = new Regex(@"\bif\s*(?:constexpr\s*)?\(", RegexOptions.Compiled);
    private static readonly Regex Identifier = new Regex(@"\b(?<name>[A-Za-z_]\w*)\b", RegexOptions.Compiled);
    private static readonly Regex Whitespace = new Regex(@"\s+", RegexOptions.Compiled);
    private static readonly Regex BooleanName = new Regex(@"\b(?:bool(?:\s+const)?\s*&?\s+|(?:std::)?atomic(?:\s*<\s*bool\s*>|_bool)\s+)(?<name>[A-Za-z_]\w*)\b", RegexOptions.Compiled);
    private static readonly Regex BooleanPrefix = new Regex(@"^(?:(?:m|s|g|k)_)?b[A-Z][A-Za-z0-9_]*$", RegexOptions.Compiled);
    private static readonly Regex Assignment = new Regex(@"\b(?<name>[A-Za-z_]\w*)\s*=(?!=)", RegexOptions.Compiled);

    private static string Mask(string text)
    {
        return LexicalContents.Replace(text, delegate(Match match)
        {
            return Regex.Replace(match.Value, @"[^\n]", " ");
        });
    }

    public static int[] FindIfViolations(string text)
    {
        text = text.Replace("\r\n", "\n").Replace('\r', '\n');
        string masked = Mask(text);

        List<int> violations = new List<int>();
        foreach (Match head in IfHead.Matches(masked))
        {
            int start = head.Index + head.Length;
            int end = start;
            int depth = 1;
            while (end < masked.Length && depth != 0)
            {
                if (masked[end] == '(')
                {
                    ++depth;
                }
                else if (masked[end] == ')')
                {
                    --depth;
                }
                ++end;
            }

            if (depth != 0)
            {
                continue;
            }
            string condition = text.Substring(start, end - start - 1);
            bool bSpansLines = condition.IndexOf('\n') >= 0;
            bool bExceedsDecisionBudget = Whitespace.Replace(condition.Trim(), " ").Length > 120;
            if (!bSpansLines && !bExceedsDecisionBudget)
            {
                continue;
            }

            string decision = masked.Substring(start, end - start - 1);
            bool bContainsCall = ContainsCall(decision);
            if (bContainsCall)
            {
                violations.Add(GetLine(text, head.Index));
            }
        }
        return violations.ToArray();
    }

    public static int[] FindBooleanBudgetViolations(string text)
    {
        text = text.Replace("\r\n", "\n").Replace('\r', '\n');
        string masked = Mask(text);
        HashSet<string> names = new HashSet<string>(StringComparer.Ordinal);
        HashSet<int> violations = new HashSet<int>();
        foreach (Match declaration in BooleanName.Matches(masked))
        {
            names.Add(declaration.Groups["name"].Value);
            int next = declaration.Index + declaration.Length;
            while (next < masked.Length && char.IsWhiteSpace(masked[next]))
            {
                ++next;
            }
            bool bBraced = next < masked.Length && masked[next] == '{';
            bool bMutableAlias = BindsMutableReference(masked, declaration.Groups["name"].Index);
            if (bBraced && !bMutableAlias)
            {
                int start = next + 1;
                int end = FindExpressionEnd(masked, start);
                int checks = CountChecks(masked.Substring(start, end - start));
                if (checks > 2)
                {
                    violations.Add(GetLine(text, declaration.Index));
                }
            }
        }

        foreach (Match assignment in Assignment.Matches(masked))
        {
            bool bMutableAlias = BindsMutableReference(masked, assignment.Index);
            if (bMutableAlias)
            {
                continue;
            }
            string name = assignment.Groups["name"].Value;
            bool bBooleanName = names.Contains(name) || BooleanPrefix.IsMatch(name);
            if (!bBooleanName)
            {
                continue;
            }
            int previous = assignment.Index - 1;
            while (previous >= 0 && char.IsWhiteSpace(masked[previous]))
            {
                --previous;
            }
            char qualifier = previous >= 0 ? masked[previous] : '\0';
            bool bQualified = ".>:".IndexOf(qualifier) >= 0;
            if (bQualified && !BooleanPrefix.IsMatch(name))
            {
                continue;
            }

            int start = assignment.Index + assignment.Length;
            int end = FindExpressionEnd(masked, start);
            int checks = CountChecks(masked.Substring(start, end - start));
            if (checks <= 2)
            {
                continue;
            }
            violations.Add(GetLine(text, assignment.Index));
        }
        int[] result = new int[violations.Count];
        violations.CopyTo(result);
        Array.Sort(result);
        return result;
    }

    public static int[] FindBooleanNameViolations(string text)
    {
        text = text.Replace("\r\n", "\n").Replace('\r', '\n');
        string masked = Mask(text);
        List<int> violations = new List<int>();
        foreach (Match declaration in BooleanName.Matches(masked))
        {
            string name = declaration.Groups["name"].Value;
            int next = declaration.Index + declaration.Length;
            while (next < masked.Length && char.IsWhiteSpace(masked[next]))
            {
                ++next;
            }
            switch (name)
            {
                case "operator":
                case "const":
                case "volatile":
                    continue;
            }
            bool bScopeAhead = next + 1 < masked.Length && masked[next] == ':';
            bool bQualifiedFunction = bScopeAhead && masked[next + 1] == ':';
            char following = next < masked.Length ? masked[next] : '\0';
            bool bFunction = "(<".IndexOf(following) >= 0;
            if (bQualifiedFunction || bFunction)
            {
                continue;
            }
            if (BooleanPrefix.IsMatch(name))
            {
                continue;
            }
            violations.Add(GetLine(text, declaration.Index));
        }
        return violations.ToArray();
    }

    private static bool BindsMutableReference(string text, int end)
    {
        string prefix = text.Substring(0, end);
        Match reference = Regex.Match(prefix, @"\b(?<constant>const\s+)?bool\s*(?<trailingConst>const\s*)?&\s*$");
        bool bNonConst = !reference.Groups["constant"].Success && !reference.Groups["trailingConst"].Success;
        return reference.Success && bNonConst;
    }

    private static int GetLine(string text, int end)
    {
        int line = 1;
        for (int position = 0; position < end; ++position)
        {
            if (text[position] == '\n')
            {
                ++line;
            }
        }
        return line;
    }

    private static int FindExpressionEnd(string text, int start)
    {
        int parentheses = 0;
        int brackets = 0;
        int braces = 0;
        for (int position = start; position < text.Length; ++position)
        {
            char current = text[position];
            if (current == '<')
            {
                position = FindTemplateEnd(text, position);
            }
            if (parentheses == 0 && brackets == 0 && braces == 0 && (current == ';' || current == ',' || current == ')' || current == '}'))
            {
                return position;
            }
            if (current == '(')
            {
                ++parentheses;
            }
            else if (current == ')')
            {
                --parentheses;
            }
            else if (current == '[')
            {
                ++brackets;
            }
            else if (current == ']')
            {
                --brackets;
            }
            else if (current == '{')
            {
                ++braces;
            }
            else if (current == '}')
            {
                --braces;
            }
        }
        return text.Length;
    }

    private static int FindCloseParenthesis(string text, int start)
    {
        int depth = 1;
        for (int position = start + 1; position < text.Length; ++position)
        {
            if (text[position] == '(')
            {
                ++depth;
            }
            else if (text[position] == ')' && --depth == 0)
            {
                return position;
            }
        }
        return text.Length - 1;
    }

    private static int CountChecks(string expression)
    {
        expression = expression.Trim();
        if (expression.Length == 0)
        {
            return 0;
        }
        if (expression[0] == '(' && FindCloseParenthesis(expression, 0) == expression.Length - 1)
        {
            return CountChecks(expression.Substring(1, expression.Length - 2));
        }
        if (expression[0] == '!' && !expression.StartsWith("!=", StringComparison.Ordinal))
        {
            return CountChecks(expression.Substring(1));
        }

        List<string> terms = new List<string>();
        int parentheses = 0;
        int brackets = 0;
        int braces = 0;
        int start = 0;
        int question = -1;
        int conditionalDepth = 0;
        for (int position = 0; position + 1 < expression.Length; ++position)
        {
            char current = expression[position];
            if (current == '<')
            {
                position = FindTemplateEnd(expression, position);
            }
            if (current == '(')
            {
                ++parentheses;
            }
            else if (current == ')')
            {
                --parentheses;
            }
            else if (current == '[')
            {
                ++brackets;
            }
            else if (current == ']')
            {
                --brackets;
            }
            else if (current == '{')
            {
                ++braces;
            }
            else if (current == '}')
            {
                --braces;
            }
            else if (parentheses == 0 && brackets == 0 && braces == 0 && current == '?')
            {
                if (question < 0)
                {
                    question = position;
                }
                ++conditionalDepth;
            }
            else if (parentheses == 0 && brackets == 0 && braces == 0 && current == ':' && question >= 0 && expression[position + 1] != ':' && expression[position - 1] != ':')
            {
                --conditionalDepth;
                if (conditionalDepth == 0)
                {
                    return CountChecks(expression.Substring(0, question))
                        + CountChecks(expression.Substring(question + 1, position - question - 1))
                        + CountChecks(expression.Substring(position + 1));
                }
            }
            else if (question < 0 && parentheses == 0 && brackets == 0 && braces == 0 && ((current == '&' && expression[position + 1] == '&') || (current == '|' && expression[position + 1] == '|')))
            {
                string term = expression.Substring(start, position - start).Trim();
                if (term == "...")
                {
                    return 1;
                }
                terms.Add(term);
                start = position + 2;
                ++position;
            }
        }
        if (terms.Count != 0)
        {
            string final = expression.Substring(start).Trim();
            if (final == "...")
            {
                return 1;
            }
            terms.Add(final);
            int result = 0;
            foreach (string term in terms)
            {
                result += CountChecks(term);
            }
            return result;
        }

        // Predicate arguments and lambda bodies belong to their own operation.
        // Ordinary nested groups and boolean casts cannot hide extra checks.
        int nested = 1;
        for (int position = 0; position < expression.Length; ++position)
        {
            if (expression[position] != '(')
            {
                continue;
            }
            int end = FindCloseParenthesis(expression, position);
            int previous = position - 1;
            while (previous >= 0 && char.IsWhiteSpace(expression[previous]))
            {
                --previous;
            }
            char preceding = previous >= 0 ? expression[previous] : '\0';
            bool bPunctuationTail = "_>)]".IndexOf(preceding) >= 0;
            bool bInvocation = char.IsLetterOrDigit(preceding) || bPunctuationTail;
            bool bBooleanCast = Regex.IsMatch(expression.Substring(0, position), @"(?:^|\W)(?:bool|static_cast\s*<\s*bool\s*>)\s*$");
            if (!bInvocation || bBooleanCast)
            {
                nested = Math.Max(nested, CountChecks(expression.Substring(position + 1, end - position - 1)));
            }
            position = end;
        }
        return nested;
    }

    private static int FindTemplateEnd(string text, int start)
    {
        int nameStart = start;
        while (nameStart > 0)
        {
            char preceding = text[nameStart - 1];
            bool bNameCharacter = char.IsLetterOrDigit(preceding) || preceding == '_';
            if (!bNameCharacter && preceding != ':')
            {
                break;
            }
            --nameStart;
        }
        if (nameStart == start)
        {
            return start;
        }
        string name = text.Substring(nameStart, start - nameStart);
        bool bQualified = name.Contains("::");
        bool bValueTemplate = Regex.IsMatch(name, @"(?:^|::)(?:b[A-Z]\w*|\w+_v)$");
        bool bNamedTemplate = bQualified || bValueTemplate;
        int depth = 1;
        for (int position = start + 1; position < text.Length; ++position)
        {
            char current = text[position];
            if (current == '<')
            {
                ++depth;
            }
            else if (current == '>' && --depth == 0)
            {
                int next = position + 1;
                while (next < text.Length && char.IsWhiteSpace(text[next]))
                {
                    ++next;
                }
                bool bCall = next < text.Length && text[next] == '(';
                bool bMember = next + 1 < text.Length && text[next] == ':';
                bool bPostfix = bCall || bMember;
                return bNamedTemplate || bPostfix ? position : start;
            }
            else if (current == ';' || current == '{' || current == '}')
            {
                return start;
            }
        }
        return start;
    }

    private static bool ContainsCall(string decision)
    {
        foreach (Match call in Identifier.Matches(decision))
        {
            switch (call.Groups["name"].Value)
            {
                case "if":
                case "for":
                case "while":
                case "switch":
                case "catch":
                case "sizeof":
                case "alignof":
                case "decltype":
                case "noexcept":
                case "static_cast":
                case "reinterpret_cast":
                case "const_cast":
                case "dynamic_cast":
                    continue;
            }
            bool bInvoked = IsCall(decision, call.Index + call.Length);
            if (bInvoked)
            {
                return true;
            }
        }
        return false;
    }

    private static bool IsCall(string expression, int position)
    {
        while (position < expression.Length && char.IsWhiteSpace(expression[position]))
        {
            ++position;
        }
        if (position < expression.Length && expression[position] == '<')
        {
            int depth = 1;
            ++position;
            while (position < expression.Length && depth != 0)
            {
                char current = expression[position++];
                if (current == '<')
                {
                    ++depth;
                }
                else if (current == '>')
                {
                    --depth;
                }
                else if (current == ';' || current == '{' || current == '}')
                {
                    return false;
                }
            }
            if (depth != 0)
            {
                return false;
            }
            while (position < expression.Length && char.IsWhiteSpace(expression[position]))
            {
                ++position;
            }
        }
        return position < expression.Length && expression[position] == '(';
    }
}
