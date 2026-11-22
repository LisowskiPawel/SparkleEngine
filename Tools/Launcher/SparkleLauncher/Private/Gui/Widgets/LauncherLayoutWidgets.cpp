#include "Gui/Widgets/LauncherLayoutWidgets.h"

#include "Gui/Widgets/LauncherArtworkWidgets.h"
#include "Gui/Style/LauncherUiDesign.h"

#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QLayout>

#include <algorithm>
#include <cmath>
#include <utility>

namespace SparkleLauncher
{
	ElidedLabel::ElidedLabel(QString fullText, QWidget* parent) :
	    QLabel(parent),
	    m_fullText(std::move(fullText))
	{
		setText(m_fullText);
		setToolTip(m_fullText);
		setAccessibleName(m_fullText);
	}

	void ElidedLabel::resizeEvent(QResizeEvent* event)
	{
		QLabel::resizeEvent(event);
		RefreshDisplayedText();
	}

	void ElidedLabel::RefreshDisplayedText()
	{
		const int availableWidth = std::max(0, contentsRect().width() - LauncherUi::Space::Medium);
		const QString displayedText = fontMetrics().elidedText(m_fullText, Qt::ElideRight, availableWidth);
		if (text() != displayedText)
		{
			setText(displayedText);
		}
	}

	SceneHeroCardWidget::SceneHeroCardWidget(QPixmap source, QWidget* parent) :
	    QFrame(parent),
	    m_source(std::move(source))
	{
		setAttribute(Qt::WA_StyledBackground, false);
		setAttribute(Qt::WA_OpaquePaintEvent, false);
		setMinimumHeight(LauncherUi::Hero::MinimumHeight);
		setMaximumHeight(QWIDGETSIZE_MAX);
		QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		policy.setHeightForWidth(true);
		setSizePolicy(policy);
	}

	void SceneHeroCardWidget::SetCopyPane(QWidget* copyPane)
	{
		m_copyPane = copyPane;
		if (m_copyPane != nullptr)
		{
			m_copyPane->setParent(this);
			m_copyPane->setMaximumWidth(QWIDGETSIZE_MAX);
			m_copyPane->show();
			LayoutCopyPane();
		}
	}

	bool SceneHeroCardWidget::hasHeightForWidth() const
	{
		return true;
	}

	int SceneHeroCardWidget::heightForWidth(int width) const
	{
		const int proportionalHeight = static_cast<int>(std::round(static_cast<double>(width) * LauncherUi::Hero::DesignHeight / LauncherUi::Hero::DesignWidth));
		return std::max(proportionalHeight, LauncherUi::Hero::MinimumHeight);
	}

	QSize SceneHeroCardWidget::sizeHint() const
	{
		return QSize(LauncherUi::Hero::DesignWidth, LauncherUi::Hero::DesignHeight);
	}

	QSize SceneHeroCardWidget::minimumSizeHint() const
	{
		return QSize(LauncherUi::Hero::MinimumWidth, LauncherUi::Hero::MinimumHeight);
	}

	void SceneHeroCardWidget::paintEvent(QPaintEvent*)
	{
		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
		painter.fillRect(rect(), LauncherUi::Color::Hex(LauncherUi::Color::Background));

		if (m_source.isNull() || width() <= 0 || height() <= 0)
		{
			return;
		}

		QImage heroLayer(QSize(LauncherUi::Hero::DesignWidth, LauncherUi::Hero::DesignHeight), QImage::Format_ARGB32_Premultiplied);
		heroLayer.fill(Qt::transparent);
		QPainter heroPainter(&heroLayer);
		heroPainter.setRenderHint(QPainter::Antialiasing, true);
		heroPainter.setRenderHint(QPainter::SmoothPixmapTransform, true);

		const QRect designRect(0, 0, LauncherUi::Hero::DesignWidth, LauncherUi::Hero::DesignHeight);
		PaintLauncherArtwork(heroPainter, designRect, m_source, LauncherArtworkSpec::ForSceneHero());

		painter.drawImage(HeroSceneRect(), heroLayer);
	}

	void SceneHeroCardWidget::resizeEvent(QResizeEvent* event)
	{
		QFrame::resizeEvent(event);
		SyncProportionalHeight();
		LayoutCopyPane();
	}

	void SceneHeroCardWidget::SyncProportionalHeight()
	{
		if (width() <= 0)
		{
			return;
		}

		const int desiredHeight = heightForWidth(width());
		if (minimumHeight() == desiredHeight && maximumHeight() == desiredHeight)
		{
			return;
		}

		setMinimumHeight(desiredHeight);
		setMaximumHeight(desiredHeight);
		updateGeometry();
	}

	void SceneHeroCardWidget::LayoutCopyPane()
	{
		if (m_copyPane == nullptr || width() <= 0 || height() <= 0)
		{
			return;
		}

		const double scale = std::clamp(HeroSceneScale(), LauncherUi::Hero::MinimumCopyScale, LauncherUi::Hero::MaximumCopyScale);
		const QRectF sceneRect = HeroSceneRect();
		const int left = static_cast<int>(std::round(sceneRect.left() + LauncherUi::Hero::CopyLeft * scale));
		const int top = static_cast<int>(std::round(sceneRect.top() + LauncherUi::Hero::CopyTop * scale));
		const int bottom = static_cast<int>(std::round(LauncherUi::Hero::CopyBottom * scale));
		const int desiredPaneWidth = static_cast<int>(std::round(LauncherUi::Hero::CopyWidth * scale));
		const double availableCopyWidth = (LauncherUi::Hero::CopyDividerX - LauncherUi::Hero::CopyLeft - 54) * scale;
		const int maximumPaneWidth = std::max(300, static_cast<int>(std::round(availableCopyWidth)));
		const int paneWidthLimit = std::min(460, maximumPaneWidth);
		const int paneWidth = std::clamp(desiredPaneWidth, 300, paneWidthLimit);
		const int sceneHeight = static_cast<int>(std::round(sceneRect.height()));
		const int topInset = static_cast<int>(std::round(LauncherUi::Hero::CopyTop * scale));
		const int paneHeight = std::max(140, sceneHeight - topInset - bottom);

		if (QLayout* copyLayout = m_copyPane->layout())
		{
			copyLayout->setContentsMargins(0, 0, 0, 0);
			copyLayout->setSpacing(std::max(10, static_cast<int>(std::round(18 * scale))));
		}
		m_copyPane->setGeometry(left, top, paneWidth, paneHeight);
	}

	QRectF SceneHeroCardWidget::HeroSceneRect() const
	{
		if (width() <= 0 || height() <= 0)
		{
			return QRectF();
		}

		const double scale = HeroSceneScale();
		const QSizeF sceneSize(LauncherUi::Hero::DesignWidth * scale, LauncherUi::Hero::DesignHeight * scale);
		const QPointF sceneTopLeft(0.0, (static_cast<double>(height()) - sceneSize.height()) * 0.5);
		return QRectF(sceneTopLeft, sceneSize);
	}

	double SceneHeroCardWidget::HeroSceneScale() const
	{
		if (width() <= 0 || height() <= 0)
		{
			return 1.0;
		}
		return std::min(static_cast<double>(width()) / LauncherUi::Hero::DesignWidth, static_cast<double>(height()) / LauncherUi::Hero::DesignHeight);
	}

}
