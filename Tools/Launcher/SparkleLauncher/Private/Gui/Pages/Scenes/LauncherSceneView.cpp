#include "Gui/Pages/Scenes/LauncherSceneView.h"

#include "Gui/Models/LauncherLevelUiModel.h"
#include "Gui/Style/LauncherUiDesign.h"

#include <QtGui/QKeyEvent>
#include <QtGui/QKeySequence>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QAction>
#include <QtGui/QShortcut>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>
#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QStyleOptionButton>

#include <algorithm>
#include <cmath>

namespace SparkleLauncher
{
	struct SceneCardGeometry final
	{
		explicit SceneCardGeometry(QRect cell);
		QRect Card;
		QRect Artwork;
		QRect Body;
		QRect Title;
		QRect Description;
		QRect Metadata;
		QRect Source;
		QRect Primary;
	};

	SceneCardGeometry::SceneCardGeometry(QRect cell)
	{
		Card = cell.adjusted(0, 0, -LauncherUi::MapCatalog::HorizontalSpacing, -LauncherUi::MapCatalog::VerticalSpacing);
		Artwork = QRect(Card.topLeft(), QSize(Card.width() * 2 / 5, Card.height()));
		Body = Card.adjusted(Artwork.width(), 0, 0, 0).marginsRemoved(LauncherUi::MapCatalog::CardBodyMargins);
		const int actionWidth = std::min(LauncherUi::Row::StatusActionWidth, std::max(1, (Body.width() - 8) / 2));
		Primary = QRect(Body.right() - actionWidth + 1, Body.bottom() - LauncherUi::Row::StatusActionHeight + 1, actionWidth, LauncherUi::Row::StatusActionHeight);
		Source = QRect(Body.left(), Primary.top(), actionWidth, Primary.height());
		Title = QRect(Body.left(), Body.top(), Body.width(), 22);
		Metadata = QRect(Body.left(), Primary.top() - 23, Body.width(), 16);
		Description = QRect(Body.left(), Title.bottom() + 7, Body.width(), std::max(0, Metadata.top() - Title.bottom() - 14));
	}

	class SceneCardDelegate final : public QStyledItemDelegate
	{
	public:
		SceneCardDelegate(LauncherSceneView& view, LauncherLevelUiModel& model, QPushButton& source, QPushButton& primary);
		QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
		void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	private:
		LauncherSceneView& m_view;
		LauncherLevelUiModel& m_model;
		QPushButton& m_source;
		QPushButton& m_primary;
	};

	SceneCardDelegate::SceneCardDelegate(LauncherSceneView& view, LauncherLevelUiModel& model, QPushButton& source, QPushButton& primary) :
	    QStyledItemDelegate(&view),
	    m_view(view),
	    m_model(model),
	    m_source(source),
	    m_primary(primary)
	{
	}

	QSize SceneCardDelegate::sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const
	{
		return m_view.gridSize();
	}

	void SceneCardDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		const LauncherLevelUiEntry* level = m_model.Level(index);
		if (level == nullptr)
		{
			return;
		}
		const SceneCardGeometry geometry(option.rect);
		const bool bCurrent = index == m_view.currentIndex();
		const QColor accent(level->State == "warning" ? LauncherUi::Color::StateWarning : LauncherUi::Color::Accent);
		painter->save();
		painter->setClipRect(geometry.Card);
		painter->fillRect(geometry.Card, QColor(LauncherUi::Color::Panel));
		if (!level->Artwork.isNull())
		{
			const QSizeF sourceSize = level->Artwork.deviceIndependentSize();
			const double scale = std::max(geometry.Artwork.width() / sourceSize.width(), geometry.Artwork.height() / sourceSize.height());
			const QSizeF drawnSize = sourceSize * scale;
			const QPointF imageOrigin = QPointF(geometry.Artwork.center()) - QPointF(drawnSize.width() / 2, drawnSize.height() / 2);
			painter->setClipRect(geometry.Artwork, Qt::IntersectClip);
			painter->drawPixmap(QRectF(imageOrigin, drawnSize), level->Artwork, QRectF(level->Artwork.rect()));
			painter->setClipRect(geometry.Card);
		}
		painter->setPen(bCurrent ? accent : QColor(LauncherUi::Color::BorderSoft));
		painter->drawRect(geometry.Card.adjusted(0, 0, -1, -1));
		painter->fillRect(QRect(geometry.Title.topLeft(), QSize(2, geometry.Title.height())), accent);
		QFont titleFont = option.font;
		titleFont.setPointSizeF(10.5);
		titleFont.setBold(true);
		painter->setFont(titleFont);
		painter->setPen(QColor(LauncherUi::Color::TextPrimary));
		const QRect title = geometry.Title.adjusted(11, 0, 0, 0);
		painter->drawText(title, Qt::AlignVCenter, QFontMetrics(titleFont).elidedText(level->DisplayName, Qt::ElideRight, title.width()));
		QFont bodyFont = option.font;
		bodyFont.setPointSizeF(8.75);
		painter->setFont(bodyFont);
		painter->setPen(QColor(LauncherUi::Color::TextBody));
		painter->drawText(geometry.Description, Qt::TextWordWrap | Qt::AlignTop, level->Description);
		bodyFont.setPointSizeF(8);
		painter->setFont(bodyFont);
		const QFontMetrics metrics(bodyFont);
		const int statusWidth = std::min(geometry.Metadata.width(), metrics.horizontalAdvance(level->Status) + 10);
		painter->setPen(accent);
		painter->drawText(geometry.Metadata, Qt::AlignVCenter, level->Status);
		painter->setPen(QColor(LauncherUi::Color::TextMuted));
		const QRect detail = geometry.Metadata.adjusted(statusWidth, 0, 0, 0);
		painter->drawText(detail, Qt::AlignVCenter, metrics.elidedText(level->Detail, Qt::ElideRight, std::max(0, detail.width())));
		const auto drawButton = [&](QPushButton& button, QRect rect, const QString& text, bool bEnabled)
		{
			QStyleOptionButton buttonOption;
			buttonOption.initFrom(&button);
			buttonOption.rect = rect;
			buttonOption.text = text;
			buttonOption.state.setFlag(QStyle::State_Enabled, bEnabled);
			buttonOption.state.setFlag(QStyle::State_HasFocus, false);
			button.style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter, &button);
		};
		if (!level->SourcePageUrl.isEmpty())
		{
			drawButton(m_source, geometry.Source, "Source", true);
		}
		drawButton(m_primary, geometry.Primary, level->ActionText, level->Action != LauncherLevelAction::None);
		painter->restore();
	}

	LauncherSceneView::LauncherSceneView(LauncherLevelUiModel& model, QWidget* parent, const std::function<void(QWidget*)>& registerFocusable) :
	    QListView(parent),
	    m_model(model)
	{
		setObjectName("SceneCatalogView");
		setAccessibleName("Levels");
		setViewMode(QListView::IconMode);
		setFlow(QListView::LeftToRight);
		setWrapping(true);
		setResizeMode(QListView::Adjust);
		setMovement(QListView::Static);
		setEditTriggers(QAbstractItemView::NoEditTriggers);
		setDragDropMode(QAbstractItemView::NoDragDrop);
		setSelectionMode(QAbstractItemView::SingleSelection);
		setUniformItemSizes(true);
		setFrameShape(QFrame::NoFrame);
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		m_sourceButton = new QPushButton("Source", parent);
		m_primaryButton = new QPushButton(parent);
		m_sourceButton->setObjectName("MapCardSourceButton");
		m_primaryButton->setObjectName("MapCardActionButton");
		m_sourceButton->setAutoDefault(true);
		m_primaryButton->setAutoDefault(true);
		m_sourceButton->installEventFilter(this);
		m_primaryButton->installEventFilter(this);
		connect(m_sourceButton, &QPushButton::clicked, this, [this]() { Activate(LauncherLevelAction::Source); });
		connect(
		    m_primaryButton,
		    &QPushButton::clicked,
		    this,
		    [this]()
		    {
			    if (const auto* level = m_model.Level(currentIndex()))
			    {
				    Activate(level->Action);
			    }
		    });
		setItemDelegate(new SceneCardDelegate(*this, model, *m_sourceButton, *m_primaryButton));
		setModel(&model);
		connect(&model, &QAbstractItemModel::dataChanged, this, [this]() { UpdateActions(); });
		connect(
		    &model,
		    &QAbstractItemModel::modelAboutToBeReset,
		    this,
		    [this]()
		    {
			    m_resetContentId = m_model.ContentId();
			    m_previousRow = std::max(0, currentIndex().row());
			    m_pressedLevelId.clear();
		    });
		connect(
		    &model,
		    &QAbstractItemModel::modelReset,
		    this,
		    [this]()
		    {
			    const QString levelId = m_currentLevelId;
			    int row = m_resetContentId == m_model.ContentId() ? std::min(m_previousRow, m_model.rowCount() - 1) : 0;
			    for (int i = 0; m_resetContentId == m_model.ContentId() && i < m_model.rowCount(); ++i)
			    {
				    if (m_model.Level(m_model.index(i))->Id == levelId)
				    {
					    row = i;
				    }
			    }
			    UpdateCellSize();
			    setCurrentIndex(m_model.index(row));
			    UpdateActions();
		    });
		auto* activate = new QShortcut(QList<QKeySequence>{Qt::Key_Return, Qt::Key_Enter, Qt::Key_Space}, this);
		activate->setContext(Qt::WidgetShortcut);
		activate->setAutoRepeat(false);
		connect(activate, &QShortcut::activated, m_primaryButton, &QPushButton::click);
		auto* copy = new QAction("Copy details", this);
		copy->setShortcut(QKeySequence::Copy);
		copy->setShortcutContext(Qt::WidgetShortcut);
		connect(copy, &QAction::triggered, this, [this]() { Activate(LauncherLevelAction::CopyDetails); });
		addAction(copy);
		setContextMenuPolicy(Qt::ActionsContextMenu);
		UpdateCellSize();
		UpdateActions();
		registerFocusable(this);
		registerFocusable(m_sourceButton);
		registerFocusable(m_primaryButton);
	}

	LauncherSceneView::~LauncherSceneView()
	{
		delete m_sourceButton;
		delete m_primaryButton;
	}

	bool LauncherSceneView::event(QEvent* event)
	{
		const bool bHandled = QListView::event(event);
		const QEvent::Type type = event->type();
		const bool bVisibilityChanged = type == QEvent::Show || type == QEvent::Hide;
		const bool bLayoutChanged = bVisibilityChanged || type == QEvent::Move;
		if (m_sourceButton != nullptr && bLayoutChanged)
		{
			UpdateActions();
		}
		return bHandled;
	}

	void LauncherSceneView::UpdateCellSize()
	{
		const int gap = LauncherUi::MapCatalog::HorizontalSpacing;
		const int width = std::max(gap + 1, contentsRect().width());
		const int limit = std::max(1, std::min(LauncherUi::MapCatalog::MaximumColumns, m_model.rowCount()));
		const int columns = std::clamp(width / (LauncherUi::MapCatalog::MinimumCardWidth + gap), 1, limit);
		const int cellWidth = std::min(LauncherUi::MapCatalog::MaximumCardWidth + gap, width / columns);
		const int cellHeight = static_cast<int>(std::round((cellWidth - gap) / LauncherUi::MapCatalog::CardAspectRatio)) + LauncherUi::MapCatalog::VerticalSpacing;
		// Include the clipped trailing gutters and Qt's inclusive wrap edge in the viewport.
		setViewportMargins(0, 0, width - columns * cellWidth - 1, -LauncherUi::MapCatalog::VerticalSpacing);
		setGridSize(QSize(cellWidth, cellHeight));
		const int rows = (m_model.rowCount() + columns - 1) / columns;
		setFixedHeight(std::max(0, rows * cellHeight - LauncherUi::MapCatalog::VerticalSpacing));
		doItemsLayout();
		UpdateActions();
	}

	void LauncherSceneView::resizeEvent(QResizeEvent* event)
	{
		QListView::resizeEvent(event);
		UpdateCellSize();
	}

	void LauncherSceneView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
	{
		QListView::currentChanged(current, previous);
		if (const auto* level = m_model.Level(current))
		{
			m_currentLevelId = level->Id;
		}
		UpdateActions();
		if (current.isValid())
		{
			emit CurrentItemChanged(visualRect(current));
		}
	}

	void LauncherSceneView::UpdateActions()
	{
		const auto* level = m_model.Level(currentIndex());
		m_sourceButton->setVisible(level != nullptr && !level->SourcePageUrl.isEmpty() && isVisible());
		m_primaryButton->setVisible(level != nullptr && isVisible());
		if (level == nullptr)
		{
			return;
		}
		const SceneCardGeometry geometry(visualRect(currentIndex()));
		m_sourceButton->setDown(false);
		m_primaryButton->setDown(false);
		m_sourceButton->setGeometry(QRect(viewport()->mapTo(parentWidget(), geometry.Source.topLeft()), geometry.Source.size()));
		m_primaryButton->setGeometry(QRect(viewport()->mapTo(parentWidget(), geometry.Primary.topLeft()), geometry.Primary.size()));
		m_sourceButton->setAccessibleName("Source for " + level->DisplayName);
		m_sourceButton->setToolTip("Open the publisher's preview and download page.");
		m_primaryButton->setText(level->ActionText);
		m_primaryButton->setAccessibleName(level->ActionText + " " + level->DisplayName);
		m_primaryButton->setToolTip(level->ActionToolTip);
		m_primaryButton->setEnabled(level->Action != LauncherLevelAction::None);
		m_sourceButton->raise();
		m_primaryButton->raise();
	}

	void LauncherSceneView::Activate(LauncherLevelAction action)
	{
		const auto* level = m_model.Level(currentIndex());
		if (level != nullptr && action != LauncherLevelAction::None)
		{
			const QString contentId = m_model.ContentId();
			const QString levelId = level->Id;
			emit ActionRequested(contentId, levelId, action);
		}
	}

	void LauncherSceneView::mousePressEvent(QMouseEvent* event)
	{
		m_pressedLevelId.clear();
		if (event->button() == Qt::LeftButton)
		{
			if (const auto* level = m_model.Level(indexAt(event->position().toPoint())))
			{
				m_pressedLevelId = level->Id;
			}
			m_pressedContentId = m_model.ContentId();
			m_pressPosition = event->position().toPoint();
		}
		QListView::mousePressEvent(event);
	}

	void LauncherSceneView::mouseReleaseEvent(QMouseEvent* event)
	{
		QListView::mouseReleaseEvent(event);
		const QModelIndex index = indexAt(event->position().toPoint());
		const auto* level = m_model.Level(index);
		const bool bLeftRelease = event->button() == Qt::LeftButton && level != nullptr;
		const bool bSameLevel = bLeftRelease && level->Id == m_pressedLevelId;
		const bool bClick = bSameLevel && m_model.ContentId() == m_pressedContentId;
		const bool bStationary = (event->position().toPoint() - m_pressPosition).manhattanLength() < QApplication::startDragDistance();
		m_pressedLevelId.clear();
		if (!bClick || !bStationary)
		{
			return;
		}
		const SceneCardGeometry geometry(visualRect(index));
		if (!geometry.Card.contains(event->position().toPoint()))
		{
			return;
		}
		Activate(geometry.Source.contains(event->position().toPoint()) && !level->SourcePageUrl.isEmpty() ? LauncherLevelAction::Source : level->Action);
	}

	void LauncherSceneView::mouseDoubleClickEvent(QMouseEvent* event)
	{
		m_pressedLevelId.clear();
		event->accept();
	}

	bool LauncherSceneView::eventFilter(QObject* object, QEvent* event)
	{
		if (event->type() == QEvent::MouseButtonDblClick)
		{
			static_cast<QPushButton*>(object)->setDown(false);
			return true;
		}
		return QListView::eventFilter(object, event);
	}
}
