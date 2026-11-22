#pragma once

#include <QtCore/QString>
#include <QtGui/QPixmap>
#include <QtWidgets/QFrame>
#include <QtWidgets/QWidget>

class QPaintEvent;
class QResizeEvent;

namespace SparkleLauncher
{
	class SceneHeroCardWidget final : public QFrame
	{
	public:
		explicit SceneHeroCardWidget(QPixmap source, QWidget* parent = nullptr);

		void SetCopyPane(QWidget* copyPane);

		bool hasHeightForWidth() const override;
		int heightForWidth(int width) const override;
		QSize sizeHint() const override;
		QSize minimumSizeHint() const override;

	protected:
		void paintEvent(QPaintEvent* event) override;
		void resizeEvent(QResizeEvent* event) override;

	private:
		void SyncProportionalHeight();
		void LayoutCopyPane();
		QRectF HeroSceneRect() const;
		double HeroSceneScale() const;

		QPixmap m_source;
		QWidget* m_copyPane = nullptr;
	};

}
