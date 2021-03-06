/*
 * This file is part of the DSLogic-gui project.
 * DSLogic-gui is based on PulseView.
 *
 * Copyright (C) 2012 Joel Holdsworth <joel@airwebreathe.org.uk>
 * Copyright (C) 2013 DreamSourceLab <dreamsourcelab@dreamsourcelab.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


#ifndef DSLOGIC_PV_SIGNAL_H
#define DSLOGIC_PV_SIGNAL_H

#include <boost/shared_ptr.hpp>

#include <QColor>
#include <QPainter>
#include <QPen>
#include <QRect>
#include <QString>

#include <stdint.h>
#include <list>

#include "libsigrok4DSLogic/libsigrok.h"

namespace pv {

namespace data {
class SignalData;
class Logic;
class Analog;
class Group;
}

namespace decoder {
class Decoder;
}

namespace view {

class Signal
{
private:
    static const int SquareWidth;
    static const int Margin;

    static const int COLOR;
    static const int NAME;
    static const int POSTRIG;
    static const int HIGTRIG;
    static const int NEGTRIG;
    static const int LOWTRIG;
    static const int LABEL;

public:
    static const QColor dsBlue;
    static const QColor dsYellow;
    static const QColor dsRed;
    static const QColor dsGreen;
    static const QColor dsGray;
    static const QColor dsLightBlue;
    static const QColor dsLightRed;
	static const QPen SignalAxisPen;

    enum {DS_LOGIC = 0, DS_ANALOG, DS_GROUP, DS_PROTOCOL};

protected:
    Signal(QString name, int index, int type, int order);
    Signal(QString name, std::list<int> index_list, int type, int order, int sec_index);

public:
    int get_type() const;
    int get_order() const;
    void set_order(int order);

    /**
     * Gets the header area size
     */
    int get_leftWidth() const;
    int get_rightWidth() const;
    int get_headerHeight() const;
    int get_index() const;
    std::list<int> &get_index_list();
    void set_index_list(std::list<int> index_list);
    int get_sec_index() const;
    void set_sec_index(int sec_index);

	/**
	 * Gets the name of this signal.
	 */
	QString get_name() const;

	/**
	 * Sets the name of the signal.
	 */
	void set_name(QString name);

	/**
	 * Get the colour of the signal.
	 */
	QColor get_colour() const;

	/**
	 * Set the colour of the signal.
	 */
	void set_colour(QColor colour);

	/**
	 * Gets the vertical layout offset of this signal.
	 */
	int get_v_offset() const;

	/**
	 * Sets the vertical layout offset of this signal.
	 */
	void set_v_offset(int v_offset);

    /**
     * Gets the height of this signal.
     */
    int get_signalHeight() const;

    /**
     * Sets the height of this signal.
     */
    void set_signalHeight(int height);

    /**
     * Gets the old vertical layout offset of this signal.
     */
    int get_old_v_offset() const;

    /**
     * Sets the old vertical layout offset of this signal.
     */
    void set_old_v_offset(int v_offset);

	/**
	 * Returns true if the signal has been selected by the user.
	 */
	bool selected() const;

	/**
	 * Selects or deselects the signal.
	 */
	void select(bool select = true);

    /*
     *
     */
    int get_trig() const;
    void set_trig(int trig);

	/**
	 * Paints the signal with a QPainter
	 * @param p the QPainter to paint into.
	 * @param y the y-coordinate to draw the signal at
	 * @param left the x-coordinate of the left edge of the signal
	 * @param right the x-coordinate of the right edge of the signal
	 * @param scale the scale in seconds per pixel.
	 * @param offset the time to show at the left hand edge of
	 *   the view in seconds.
	 **/
	virtual void paint(QPainter &p, int y, int left, int right,
		double scale, double offset) = 0;

    virtual const std::vector< std::pair<uint64_t, bool> > cur_edges() const = 0;

    virtual void set_decoder(pv::decoder::Decoder *decoder) = 0;

    virtual pv::decoder::Decoder* get_decoder() = 0;

    virtual void del_decoder() = 0;

    virtual void set_data(boost::shared_ptr<pv::data::Logic> _logic_data,
                          boost::shared_ptr<pv::data::Analog> _analog_data,
                          boost::shared_ptr<pv::data::Group> _group_data) = 0;

	/**
	 * Paints the signal label into a QGLWidget.
	 * @param p the QPainter to paint into.
	 * @param y the y-coordinate of the signal.
	 * @param right the x-coordinate of the right edge of the header
	 * 	area.
	 * @param hover true if the label is being hovered over by the mouse.
	 */
	virtual void paint_label(QPainter &p, int y, int right,
        bool hover, int action);

    /**
     * Determines if a point is in the header rect.
     * 1 - in color rect
     * 2 - in name rect
     * 3 - in posTrig rect
     * 4 - in higTrig rect
     * 5 - in negTrig rect
     * 6 - in lowTrig rect
     * 7 - in label rect
     * 0 - not
     * @param y the y-coordinate of the signal.
     * @param right the x-coordinate of the right edge of the header
     * 	area.
     * @param point the point to test.
     */
    int pt_in_rect(int y, int right,
        const QPoint &point);

protected:

	/**
	 * Paints a zero axis across the viewport.
	 * @param p the QPainter to paint into.
	 * @param y the y-offset of the axis.
	 * @param left the x-coordinate of the left edge of the view.
	 * @param right the x-coordinate of the right edge of the view.
	 */
	void paint_axis(QPainter &p, int y, int left, int right);

private:

	/**
	 * Computes an caches the size of the label text.
	 */
	void compute_text_size(QPainter &p);

    /**
     * Computes the outline rectangle of a label.
     * @param p the QPainter to lay out text with.
     * @param y the y-coordinate of the signal.
     * @param right the x-coordinate of the right edge of the header
     * 	area.
     * @return Returns the rectangle of the signal label.
     */
    QRectF get_rect(const char *s, int y, int right);

protected:
    int _type;
    std::list<int> _index_list;
    int _order;
    int _sec_index;
	QString _name;
	QColor _colour;
	int _v_offset;
    int _old_v_offset;

    int _signalHeight;

	bool _selected;

    int _trig;

	QSizeF _text_size;
};

} // namespace view
} // namespace pv

#endif // DSLOGIC_PV_SIGNAL_H
