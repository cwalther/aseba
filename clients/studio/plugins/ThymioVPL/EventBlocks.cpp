/*
	Aseba - an event-based framework for distributed robot control
	Copyright (C) 2007--2015:
		Stephane Magnenat <stephane at magnenat dot net>
		(http://stephane.magnenat.net)
		and other contributors, see authors.txt for details
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published
	by the Free Software Foundation, version 3 of the License.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <QPainter>
#include <QSvgRenderer>
#include <QTimeLine>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSvgItem>
#include <QSlider>
#include <QGraphicsProxyWidget>
#include <QGraphicsEllipseItem>
#include <QtCore/qmath.h>
#include <QDebug>

#include "EventBlocks.h"
#include "Buttons.h"
#include "UsageLogger.h"
#include "Style.h"
#include "Scene.h"

#define deg2rad(x) ((x) * M_PI / 180.)

namespace Aseba { namespace ThymioVPL
{
	// Buttons Event
	ArrowButtonsEventBlock::ArrowButtonsEventBlock(QGraphicsItem *parent) : 
		BlockWithButtons("event", "button", true, parent)
	{
		const QColor color(Qt::red);
		
		// top, left, bottom, right
		for(int i=0; i<4; i++) 
		{
			GeometryShapeButton *button = new GeometryShapeButton(QRectF(-25, -21.5, 50, 43), GeometryShapeButton::TRIANGULAR_BUTTON, this, Style::unusedButtonFillColor,  Style::unusedButtonStrokeColor);

			qreal offset = (qreal)i;
			button->setRotation(-90*offset);
			button->setPos(128 - 70*qSin(1.57079633*offset), 
						   128 - 70*qCos(1.57079633*offset));
			button->addState(color);
			buttons.push_back(button);

			connect(button, SIGNAL(stateChanged()), SIGNAL(contentChanged()));
			connect(button, SIGNAL(stateChanged()), SIGNAL(undoCheckpoint()));
			USAGE_LOG(logSignal(button,SIGNAL(stateChanged()),i,this));
		}

		GeometryShapeButton *button = new GeometryShapeButton(QRectF(-25, -25, 50, 50), GeometryShapeButton::CIRCULAR_BUTTON, this, Style::unusedButtonFillColor, Style::unusedButtonStrokeColor);
		button->setPos(QPointF(128, 128));
		button->addState(color);
		buttons.push_back(button);
		connect(button, SIGNAL(stateChanged()), SIGNAL(contentChanged()));
		connect(button, SIGNAL(stateChanged()), SIGNAL(undoCheckpoint()));
		USAGE_LOG(logSignal(button,SIGNAL(stateChanged()),4,this));
	}
	
	// Prox Event
	ProxEventBlock::ProxEventBlock(bool advanced, QGraphicsItem *parent) : 
		BlockWithButtonsAndRange("event", "prox", true, PIXEL_TO_VAL_SQUARE, 700, 4000, 1000, 2000, Qt::black, Qt::white, advanced, parent)
	{
		buttonsCountSimple = 3;
		
		// indication LEDs for front sensors
		indicationLEDs.push_back(createIndicationLED(15,78));
		indicationLEDs.push_back(createIndicationLED(54,43));
		QGraphicsItemGroup* frontIndicationLEDs(new QGraphicsItemGroup(this));
		frontIndicationLEDs->addToGroup(createIndicationLED(104,26));
		frontIndicationLEDs->addToGroup(createIndicationLED(152,26));
		indicationLEDs.push_back(frontIndicationLEDs);
		indicationLEDs.push_back(createIndicationLED(202,43));
		indicationLEDs.push_back(createIndicationLED(241,78));
		// indication LEDs for back sensors
		indicationLEDs.push_back(createIndicationLED(40,234));
		indicationLEDs.push_back(createIndicationLED(216,234));
		
		// front sensors
		for(int i=0; i<5; ++i) 
		{
			GeometryShapeButton *button = new GeometryShapeButton(QRectF(-16,-16,32,32), GeometryShapeButton::RECTANGULAR_BUTTON, this, Style::unusedButtonFillColor,  Style::unusedButtonStrokeColor);
			
			const qreal offset = (qreal)2-i;
			button->setRotation(-20*offset);
			button->setPos(128 - 150*qSin(0.34906585*offset) , 
						   175 - 150*qCos(0.34906585*offset) );
			button->addState(Qt::white, Qt::red);
			button->addState(Qt::black, Qt::black);
			button->addState(Qt::darkGray, QColor(128,0,0));
			if (!advanced)
				button->setStateCountLimit(buttonsCountSimple);

			buttons.push_back(button);
			
			connect(button, SIGNAL(stateChanged()), SIGNAL(contentChanged()));
			connect(button, SIGNAL(stateChanged()), SIGNAL(undoCheckpoint()));
			connect(button, SIGNAL(stateChanged()), SLOT(updateIndicationLEDsOpacity()));
			USAGE_LOG(logSignal(button,SIGNAL(stateChanged()),i,this));
		}
		// back sensors
		for(int i=0; i<2; ++i) 
		{
			GeometryShapeButton *button = new GeometryShapeButton(QRectF(-16,-16,32,32), GeometryShapeButton::RECTANGULAR_BUTTON, this, Style::unusedButtonFillColor,  Style::unusedButtonStrokeColor);

			button->setPos(QPointF(64 + i*128, 234));
			button->addState(Qt::white, Qt::red);
			button->addState(Qt::black, Qt::black);
			button->addState(Qt::darkGray, QColor(128,0,0));
			if (!advanced)
				button->setStateCountLimit(buttonsCountSimple);
			
			buttons.push_back(button);
			
			connect(button, SIGNAL(stateChanged()), SIGNAL(contentChanged()));
			connect(button, SIGNAL(stateChanged()), SIGNAL(undoCheckpoint()));
			connect(button, SIGNAL(stateChanged()), SLOT(updateIndicationLEDsOpacity()));
			USAGE_LOG(logSignal(button,SIGNAL(stateChanged()),i+5,this));
		}
		
		updateIndicationLEDsOpacity();
	}
	
	
	// Prox Ground Event
	ProxGroundEventBlock::ProxGroundEventBlock(bool advanced, QGraphicsItem *parent) : 
		BlockWithButtonsAndRange("event", "proxground", false, PIXEL_TO_VAL_LINEAR, 0, 1023, 400, 450, Qt::black, Qt::white, advanced, parent)
	{
		buttonsCountSimple = 3;
		
		// indication LEDs
		indicationLEDs.push_back(createIndicationLED(72,40));
		indicationLEDs.push_back(createIndicationLED(184,40));
		
		// sensors
		for(int i=0; i<2; ++i) 
		{
			GeometryShapeButton *button = new GeometryShapeButton(QRectF(-16,-16,32,32), GeometryShapeButton::RECTANGULAR_BUTTON, this, Style::unusedButtonFillColor,  Style::unusedButtonStrokeColor);

			button->setPos(QPointF(98 + i*60, 40));
			button->addState(Qt::white, Qt::red);
			button->addState(Qt::black, Qt::black);
			button->addState(Qt::darkGray, QColor(128,0,0));
			if (!advanced)
				button->setStateCountLimit(buttonsCountSimple);
			
			buttons.push_back(button);
			
			connect(button, SIGNAL(stateChanged()), SIGNAL(contentChanged()));
			connect(button, SIGNAL(stateChanged()), SIGNAL(undoCheckpoint()));
			connect(button, SIGNAL(stateChanged()), SLOT(updateIndicationLEDsOpacity()));
			USAGE_LOG(logSignal(button,SIGNAL(stateChanged()),i,this));
		}
		
		updateIndicationLEDsOpacity();
	}
	
	
	// Acc Event
	const QRectF AccEventBlock::buttonPoses[3] = {
		QRectF(32+(40+36)*0, 200, 40, 40),
		QRectF(32+(40+36)*1, 200, 40, 40),
		QRectF(32+(40+36)*2, 200, 40, 40)
	};
	const int AccEventBlock::resolution = 6;
	
	AccEventBlock::AccEventBlock(bool advanced, QGraphicsItem *parent) :
		Block("event", "acc", parent),
		mode(MODE_TAP),
		orientation(0),
		dragging(false),
		tapSimpleSvg(new QGraphicsSvgItem (":/images/vpl_block_acc_tap_simple.svgz", this)),
		tapAdvancedSvg(new QGraphicsSvgItem (":/images/vpl_block_acc_tap_advanced.svgz", this)),
		quadrantSvg(new QGraphicsSvgItem (":/images/vpl_block_acc_quadrant.svgz", this)),
		pitchSvg(new QGraphicsSvgItem (":/images/vpl_block_acc_pitch.svgz", this)),
		rollSvg(new QGraphicsSvgItem (":/images/vpl_block_acc_roll.svgz", this))
	{
		tapSimpleSvg->setVisible(!advanced);
		tapAdvancedSvg->setVisible(advanced);
		quadrantSvg->setVisible(false);
		pitchSvg->setVisible(false);
		pitchSvg->setTransformOriginPoint(128, 128);
		rollSvg->setVisible(false);
		rollSvg->setTransformOriginPoint(128, 128);
	}
	
	void AccEventBlock::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
	{
		Q_UNUSED(option);
		Q_UNUSED(widget);
		
		// paint parent
		Block::paint(painter, option, widget);
		
		// if simple mode, simply return
		if (tapSimpleSvg->isVisible())
			return;
		
		// draw buttons
		for (unsigned i=0; i<3; ++i)
		{
			if (mode == i)
			{
				painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
				painter->setBrush(Qt::red);
			}
			else
			{
				painter->setPen(QPen(Style::unusedButtonStrokeColor, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
				painter->setBrush(Style::unusedButtonFillColor);
			}
			painter->drawEllipse(buttonPoses[i]);
		}
	}
	
	int AccEventBlock::getValue(unsigned i) const
	{
		if (i == 0)
			return mode;
		else
			return orientation;
	}
	
	void AccEventBlock::setValue(unsigned i, int value)
	{
		if (i == 0)
			setMode(value);
		else
			setOrientation(value);
	}
	
	QVector<quint16> AccEventBlock::getValuesCompressed() const
	{
		return QVector<quint16>(1, (orientation<<2) | mode);
	}
	
	void AccEventBlock::mousePressEvent(QGraphicsSceneMouseEvent * event)
	{
		if (event->button() == Qt::LeftButton)
		{
			for (unsigned i=0; i<3; ++i)
			{
				if (buttonPoses[i].contains(event->pos()))
				{
					setMode(i);
					USAGE_LOG(logAccEventBlockMode(this->name, this->type,i));
					emit undoCheckpoint();
					return;
				}
			}
		}
		
		if (mode != MODE_TAP)
		{
			bool ok;
			const int orientation(orientationFromPos(event->pos(), &ok));
			if (ok)
			{
				clearChangedFlag();
				setOrientation(orientation);
				dragging = true;
				return;
			}
		}
		
		Block::mousePressEvent(event);
	}
	
	void AccEventBlock::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
	{
		if (dragging)
		{
			bool ok;
			const int orientation(orientationFromPos(event->pos(), &ok));
			if (ok)
				setOrientation(orientation);
		}
		else
			Block::mouseMoveEvent(event);
	}
	
	void AccEventBlock::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
	{
		if (dragging)
		{
			dragging = false;
			emitUndoCheckpointAndClearIfChanged();
		}
		else
			Block::mouseReleaseEvent(event);
	}
	
	int AccEventBlock::orientationFromPos(const QPointF& pos, bool* ok) const
	{
		const QPointF localPos(pos-QPointF(128,128));
		if (sqrt(localPos.x()*localPos.x()+localPos.y()*localPos.y()) <= 124)
		{
			const qreal angle(atan2(-localPos.y(), localPos.x()) - M_PI/2.);
			if (angle > deg2rad(-97.5) && angle < deg2rad(97.5))
			{
				if (ok) *ok = true;
				return int(round((angle * 2 * resolution) / M_PI));
			}
		}
		if (ok) *ok = false;
		return 0;
	}
	
	void AccEventBlock::setMode(unsigned mode)
	{
		if (tapSimpleSvg->isVisible())
			return;
		if (mode != this->mode)
		{
			this->mode = mode;
			
			if (mode == MODE_TAP)
			{
				pitchSvg->setRotation(0);
				rollSvg->setRotation(0);
				orientation = 0;
			}
			tapAdvancedSvg->setVisible(mode == MODE_TAP);
			quadrantSvg->setVisible(mode != MODE_TAP);
			pitchSvg->setVisible(mode == MODE_PITCH);
			rollSvg->setVisible(mode == MODE_ROLL);
			
			update();
			emit contentChanged();
		}
	}
	
	void AccEventBlock::setOrientation(int orientation)
	{
		if (orientation != this->orientation)
		{
			this->orientation = orientation;
			
			pitchSvg->setRotation(-orientation * 90 / resolution);
			rollSvg->setRotation(-orientation * 90 / resolution);
			
			update();
			emit contentChanged();
			setChangedFlag();
		}
	}
	
	bool AccEventBlock::isAnyAdvancedFeature() const
	{
		return (!tapSimpleSvg->isVisible() && !tapAdvancedSvg->isVisible());
	}
	
	void AccEventBlock::setAdvanced(bool advanced)
	{
		if (!advanced)
		{
			setMode(0);
			tapAdvancedSvg->setVisible(false);
			tapSimpleSvg->setVisible(true);
		}
		else
		{
			tapAdvancedSvg->setVisible(true);
			tapSimpleSvg->setVisible(false);
		}
	}
	
	// Clap Event
	ClapEventBlock::ClapEventBlock( QGraphicsItem *parent ) :
		BlockWithNoValues("event", "clap", parent)
	{
		new QGraphicsSvgItem (":/images/thymioclap.svgz", this);
	}
	
	
	// TimeoutEventBlock
	TimeoutEventBlock::TimeoutEventBlock(QGraphicsItem *parent):
		BlockWithNoValues("event", "timeout", parent)
	{
		new QGraphicsSvgItem (":/images/timeout.svgz", this);
	}
	
	
	// PeriodicEventBlock
	PeriodicEventBlock::PeriodicEventBlock(QGraphicsItem *parent):
		Block("event", "periodic", parent),
		period(1000),
		lastPressedIn(false)
	{
		svgRenderer = new QSvgRenderer(QString(":/images/heartbeat.svgz"), this);
		
		timer = new QTimeLine(period, this);
		timer->setCurveShape(QTimeLine::LinearCurve);
		timer->setLoopCount(0);
		connect(timer, SIGNAL(valueChanged(qreal)), SLOT(valueChanged(qreal)));
	}
	
	// one-dimensional Bezier interpolation x(t) in n segments given by n+1 t coordinates of segment endpoints and 3*n+1 x coordinates of control points
	static qreal bezier(unsigned int n, const qreal* ts, const qreal* xs, qreal t) {
		// if out of bounds, clamp to edge values
		if (t <= ts[0]) return xs[0];
		if (t >= ts[n]) return xs[3*n];
		// binary search for segment index l such that ts[l] <= t < ts[l+1]
		unsigned int l = 0, h = n;
		while (h - l > 1)
		{
			unsigned int m = (h+l)/2;
			if (t < ts[m]) h = m;
			else l = m;
		}
		// compute segment-internal parameter st
		qreal sl = ts[l+1] - ts[l];
		qreal st = (sl == 0) ? 0 : (t - ts[l])/sl;
		// interpolate according to De Casteljau
		qreal mst = 1.0 - st;
		qreal xm = mst*xs[3*l] + st*xs[3*l+1];
		qreal xn = mst*xs[3*l+1] + st*xs[3*l+2];
		qreal xo = mst*xs[3*l+2] + st*xs[3*l+3];
		qreal xp = mst*xm + st*xn;
		qreal xq = mst*xn + st*xo;
		return mst*xp + st*xq;
	}
	
	void PeriodicEventBlock::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
	{
		static const qreal heartbeatX[] = {1.0, 1.0, 1.2, 1.2, 1.2, 0.95, 0.95, 0.95, 1.0, 1.0};
		static const qreal heartbeatT[] = {0.0, 0.1, 0.25, 0.4};
		
		Block::paint(painter, option, widget);
		
		if (timer->state() == QTimeLine::Running)
		{
			// animated size of beating heart
			qreal t = timer->currentValue();
			// for periods longer than 1 second, restrict the heartbeat to the first second
			if (period > 1000) t *= period/1000;
			qreal hw = 128*bezier(sizeof(heartbeatT)/sizeof(heartbeatT[0]) - 1, heartbeatT, heartbeatX, t);
			
			// draw timer wedge
			QConicalGradient wedgeGradient(128, 96, timer->currentValue()*360 + 90);
			wedgeGradient.setColorAt(0, Qt::red);
			wedgeGradient.setColorAt(0.98, Qt::white);
			wedgeGradient.setColorAt(1, Qt::red);
			painter->fillRect(QRectF(40, 24, 176, 144), QBrush(wedgeGradient));
			
			QConicalGradient sectorGradient(128, 96, 0);
			QColor white(255, 255, 255, 64);
			QColor red(255, 0, 0, 64);
			sectorGradient.setColorAt(0, white);
			sectorGradient.setColorAt(0.24999, red);
			sectorGradient.setColorAt(0.25001, white);
			sectorGradient.setColorAt(0.49999, red);
			sectorGradient.setColorAt(0.50001, white);
			sectorGradient.setColorAt(0.74999, red);
			sectorGradient.setColorAt(0.75001, white);
			sectorGradient.setColorAt(1, red);
			painter->fillRect(QRectF(40, 24, 176, 144), QBrush(sectorGradient));
			
			// draw heart shape
			svgRenderer->render(painter, QRectF(128-hw, 96-3*hw/4, 2*hw, 2*hw));
		}
		else
		{
			// draw stationary heart
			qreal hw = 128;
			painter->fillRect(QRectF(40, 24, 176, 144), QBrush(Qt::white));
			svgRenderer->render(painter, QRectF(128-hw, 96-3*hw/4, 2*hw, 2*hw));
		}
		
		// draw slider
		QRectF rect = rangeRect();
		const int x(rect.x());
		const int y(rect.y());
		const int lowPos(valToPixel(period));
		painter->setPen(QPen(Style::unusedButtonStrokeColor, 4, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
		painter->setBrush(Style::unusedButtonFillColor);
		painter->drawRect(rect);
		painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
		painter->setBrush(Qt::white);
		QPolygon lowCursor;
		lowCursor << QPoint(x+lowPos, y+2) << QPoint(x+lowPos+23, y+2+23) << QPoint(x+lowPos+23, y+48) << QPoint(x+lowPos-23, y+48) << QPoint(x+lowPos-23, y+2+23);
		painter->drawConvexPolygon(lowCursor);
	}
	
	void PeriodicEventBlock::valueChanged(qreal value)
	{
		update();
	}
	
	void PeriodicEventBlock::mousePressEvent(QGraphicsSceneMouseEvent * event)
	{
		lastPressedIn = (event->button() == Qt::LeftButton && rangeRect().contains(event->pos()));
		if (lastPressedIn)
		{
			timer->start();
			mouseMoveEvent(event);
		}
		else
			Block::mousePressEvent(event);
	}
	
	void PeriodicEventBlock::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
	{
		if (!lastPressedIn)
		{
			Block::mouseMoveEvent(event);
			return;
		}
		
		QPointF pos(event->pos());
		pos -= rangeRect().topLeft();
		setPeriod(pixelToVal(pos.x()));
	}
	
	void PeriodicEventBlock::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
	{
		timer->stop();
		update();
		Block::mouseReleaseEvent(event);
		
		USAGE_LOG(logBlockMouseRelease(this->name, this->type, event));
		emit undoCheckpoint();
	}
	
	QVariant PeriodicEventBlock::itemChange(GraphicsItemChange change, const QVariant & value)
	{
		if (change == ItemSceneChange)
		{
			Scene* oldScene = qobject_cast<Scene*>(scene());
			if (oldScene)
			{
				disconnect(this, SIGNAL(periodChanged(unsigned)), oldScene, SLOT(synchronizeTimer1Period(unsigned)));
				disconnect(oldScene, SIGNAL(timer1PeriodChanged(unsigned)), this, SLOT(setPeriod(unsigned)));
			}
			Scene* newScene = qobject_cast<Scene*>(value.value<QGraphicsScene*>());
			if (newScene)
			{
				connect(newScene, SIGNAL(timer1PeriodChanged(unsigned)), SLOT(setPeriod(unsigned)));
				// ask any potential other PeriodicEventBlocks to impose their current period on us
				newScene->synchronizeTimer1Period(0);
				newScene->connect(this, SIGNAL(periodChanged(unsigned)), SLOT(synchronizeTimer1Period(unsigned)));
			}
			return value;
		}
		return Block::itemChange(change, value);
	}
	
	int PeriodicEventBlock::getValue(unsigned i) const
	{
		return period;
	}
	
	void PeriodicEventBlock::setValue(unsigned i, int value) 
	{
		setPeriod(value);
	}
	
	QVector<quint16> PeriodicEventBlock::getValuesCompressed() const
	{
		return QVector<quint16>(1, getValue(0));
	}
	
	void PeriodicEventBlock::setPeriod(unsigned period)
	{
		if (period == 0)
		{
			// we're being asked for our current period
			emit periodChanged(this->period);
		}
		else if (period != this->period)
		{
			unsigned oldperiod = this->period;
			this->period = period;
			
			update();
			emit periodChanged(period);
			emit contentChanged();
			setChangedFlag();
			
			int t = timer->currentTime();
			timer->setDuration(period);
			// preserve phase
			timer->setCurrentTime(period*t/oldperiod);
			// this is needed to work around weird behavior of setCurrentTime() which would otherwise be undone at the next timer tick
			timer->setDirection(QTimeLine::Forward);
		}
	}
	
	QRectF PeriodicEventBlock::rangeRect() const
	{
		return QRectF(32, 184, 192, 48);
	}
	
	float PeriodicEventBlock::pixelToVal(float pixel) const
	{
		static const int multipliers[] = {1, 2, 5};
		float valLog = log10f(10000) - (pixel - 23)/(rangeRect().width() - 46)*(log10f(10000)-log10f(50));
		valLog = std::max(std::min(valLog, log10f(10000)), log10f(50));
		int valLogRounded = (int)floorf(3*valLog + 0.5f);
		return powf(10, valLogRounded/3)*multipliers[valLogRounded % 3];
	}
	
	float PeriodicEventBlock::valToPixel(float val) const
	{
		return 23 + (rangeRect().width() - 46)*((log10f(10000)-log10f(val))/(log10f(10000)-log10f(50)));
	}
	
} } // namespace ThymioVPL / namespace Aseba
