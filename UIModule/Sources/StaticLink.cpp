#include "StaticLink.h"

namespace ui
{
	namespace controls
	{
		StaticLink::StaticLink(QWidget* parent)
			:QLabel(parent)
		{
		}
		
		StaticLink::~StaticLink()
		{

		}

		void StaticLink::mousePressEvent(QMouseEvent * event)
		{
			emit clicked();
		}
	}
}