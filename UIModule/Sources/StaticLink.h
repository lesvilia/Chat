#pragma once
#include <QLabel>

namespace ui
{
	namespace controls
	{
		class StaticLink
			: public QLabel
		{
			Q_OBJECT
		public:
			StaticLink(QWidget* parent = nullptr);
			~StaticLink();

		signals:
			void clicked();

		protected:
			virtual void mousePressEvent(QMouseEvent* event);
		};

	}
}