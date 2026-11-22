#pragma once

#include <QtWidgets/QListView>
#include <QtCore/QPersistentModelIndex>

#include <functional>
#include <cstdint>

class QPushButton;

namespace SparkleLauncher
{
	class LauncherLevelUiModel;
	enum class LauncherLevelAction : std::uint8_t;

	class LauncherSceneView final : public QListView
	{
		Q_OBJECT

	public:
		LauncherSceneView(LauncherLevelUiModel& model, QWidget* parent, const std::function<void(QWidget*)>& registerFocusable);
		~LauncherSceneView() override;

	signals:
		void ActionRequested(const QString& contentId, const QString& levelId, LauncherLevelAction action);
		void CurrentItemChanged(const QRect& rect);

	protected:
		bool event(QEvent* event) override;
		void resizeEvent(QResizeEvent* event) override;
		void currentChanged(const QModelIndex& current, const QModelIndex& previous) override;
		void mousePressEvent(QMouseEvent* event) override;
		void mouseReleaseEvent(QMouseEvent* event) override;
		void mouseDoubleClickEvent(QMouseEvent* event) override;
		bool eventFilter(QObject* object, QEvent* event) override;

	private:
		void UpdateCellSize();
		void UpdateActions();
		void Activate(LauncherLevelAction action);

		LauncherLevelUiModel& m_model;
		QPushButton* m_sourceButton = nullptr;
		QPushButton* m_primaryButton = nullptr;
		QString m_currentLevelId;
		QString m_resetContentId;
		int m_previousRow = 0;
		QPersistentModelIndex m_pressedIndex;
		QPoint m_pressPosition;
	};
}
