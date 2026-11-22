#pragma once

#include <QtCore/QRectF>
#include <QtCore/QVector>
#include <QtGui/QColor>
#include <QtGui/QPixmap>

#include <cstdint>

class QPainter;

namespace SparkleLauncher
{
	enum class LauncherArtworkGradientAxis : std::uint8_t
	{
		Horizontal,
		Vertical,
		DiagonalDown,
	};

	struct LauncherArtworkGradientStop
	{
		qreal Position = 0.0;
		QColor Color;
	};

	struct LauncherArtworkGradientLayer
	{
		LauncherArtworkGradientAxis Axis = LauncherArtworkGradientAxis::Horizontal;
		QRectF NormalizedArea = QRectF(0.0, 0.0, 1.0, 1.0);
		QVector<LauncherArtworkGradientStop> Stops;
	};

	struct LauncherArtworkSpec
	{
		QColor BaseColor;
		double AspectRatio = 16.0 / 9.0;
		double AccentLineX = -1.0;
		qreal AccentLineWidth = 2.0;
		QColor AccentLineColor;
		QVector<LauncherArtworkGradientLayer> Layers;
		QVector<LauncherArtworkGradientLayer> TopLayers;

		static LauncherArtworkSpec ForSceneHero();
	};

	void PaintLauncherArtwork(QPainter& painter, const QRectF& targetRect, const QPixmap& source, const LauncherArtworkSpec& spec);

}
