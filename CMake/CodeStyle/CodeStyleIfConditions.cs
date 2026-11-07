using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;

public static class CodeStyleIfConditions
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
    private static readonly Regex BooleanName = new Regex(@"\bbool\s+(?<name>[A-Za-z_]\w*)\b", RegexOptions.Compiled);
    private static readonly Regex Assignment = new Regex(@"\b(?<name>[A-Za-z_]\w*)\s*=(?!=)", RegexOptions.Compiled);

    private static string Mask(string text)
    {
        return LexicalContents.Replace(text, delegate(Match match)
        {
            return Regex.Replace(match.Value, @"[^\n]", " ");
        });
    }

    public static int[] FindViolations(string text)
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
            bool spansLines = condition.IndexOf('\n') >= 0;
            bool exceedsDecisionBudget = Whitespace.Replace(condition.Trim(), " ").Length > 120;
            if (!spansLines && !exceedsDecisionBudget)
            {
                continue;
            }

            bool containsCall = false;
            string decision = masked.Substring(start, end - start - 1);
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
                bool invoked = IsCall(decision, call.Index + call.Length);
                if (invoked)
                {
                    containsCall = true;
                    break;
                }
            }

            if (containsCall)
            {
                int line = 1;
                for (int position = 0; position < head.Index; ++position)
                {
                    if (text[position] == '\n')
                    {
                        ++line;
                    }
                }
                violations.Add(line);
            }
        }
        return violations.ToArray();
    }

    public static int[] FindBooleanBudgetViolations(string text)
    {
        text = text.Replace("\r\n", "\n").Replace('\r', '\n');
        string masked = Mask(text);
        HashSet<string> names = new HashSet<string>(StringComparer.Ordinal);
        foreach (Match declaration in BooleanName.Matches(masked))
        {
            names.Add(declaration.Groups["name"].Value);
        }

        HashSet<int> violations = new HashSet<int>();
        foreach (Match assignment in Assignment.Matches(masked))
        {
            bool booleanName = names.Contains(assignment.Groups["name"].Value);
            if (!booleanName)
            {
                continue;
            }
            int previous = assignment.Index - 1;
            while (previous >= 0 && char.IsWhiteSpace(masked[previous]))
            {
                --previous;
            }
            if (previous >= 0 && (masked[previous] == '.' || masked[previous] == '>' || masked[previous] == ':'))
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
            int line = 1;
            for (int position = 0; position < assignment.Index; ++position)
            {
                if (text[position] == '\n')
                {
                    ++line;
                }
            }
            violations.Add(line);
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
        Regex declarations = new Regex(@"\b(?:bool\s+|(?:std::)?atomic\s*<\s*bool\s*>\s+)(?<name>[A-Za-z_]\w*)\b");
        foreach (Match declaration in declarations.Matches(masked))
        {
            string name = declaration.Groups["name"].Value;
            int next = declaration.Index + declaration.Length;
            while (next < masked.Length && char.IsWhiteSpace(masked[next]))
            {
                ++next;
            }
            bool qualifiedFunction = next + 1 < masked.Length && masked[next] == ':' && masked[next + 1] == ':';
            if (name == "operator" || qualifiedFunction || (next < masked.Length && (masked[next] == '(' || masked[next] == '<')))
            {
                continue;
            }
            if (Regex.IsMatch(name, @"^(?:(?:m|s|g|k)_)?b[A-Z][A-Za-z0-9_]*$"))
            {
                continue;
            }
            int line = 1;
            for (int position = 0; position < declaration.Index; ++position)
            {
                if (text[position] == '\n')
                {
                    ++line;
                }
            }
            violations.Add(line);
        }
        return violations.ToArray();
    }

    private static int FindExpressionEnd(string text, int start)
    {
        int parentheses = 0;
        int brackets = 0;
        int braces = 0;
        for (int position = start; position < text.Length; ++position)
        {
            char current = text[position];
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
            bool invocation = previous >= 0 && (char.IsLetterOrDigit(expression[previous]) || expression[previous] == '_' || expression[previous] == '>' || expression[previous] == ')' || expression[previous] == ']');
            bool booleanCast = Regex.IsMatch(expression.Substring(0, position), @"(?:^|\W)(?:bool|static_cast\s*<\s*bool\s*>)\s*$");
            if (!invocation || booleanCast)
            {
                nested = Math.Max(nested, CountChecks(expression.Substring(position + 1, end - position - 1)));
            }
            position = end;
        }
        return nested;
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
