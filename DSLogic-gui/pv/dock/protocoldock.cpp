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


#include "protocoldock.h"
#include "../decoder/democonfig.h"
#include "../sigsession.h"

#include <QObject>
#include <QHBoxLayout>
#include <QPainter>
#include <QMessageBox>

namespace pv {
namespace dock {

ProtocolDock::ProtocolDock(QWidget *parent, SigSession &session) :
    QWidget(parent),
    _session(session)
{
    QHBoxLayout *hori_layout = new QHBoxLayout();

    _add_button = new QPushButton(this);
    _add_button->setFlat(true);
    _add_button->setIcon(QIcon::fromTheme("protocol",
                         QIcon(":/icons/add.png")));
    _del_all_button = new QPushButton(this);
    _del_all_button->setFlat(true);
    _del_all_button->setIcon(QIcon::fromTheme("protocol",
                             QIcon(":/icons/del.png")));
    _del_all_button->setCheckable(true);
    _protocol_combobox = new QComboBox(this);
    for (int i = 0; decoder::protocol_list[i] != NULL;) {
        _protocol_combobox->addItem(decoder::protocol_list[i]);
        i++;
    }
    hori_layout->addWidget(_add_button);
    hori_layout->addWidget(_del_all_button);
    hori_layout->addWidget(_protocol_combobox);
    hori_layout->addStretch(1);

    connect(_add_button, SIGNAL(clicked()),
            this, SLOT(add_protocol()));
    connect(_del_all_button, SIGNAL(clicked()),
            this, SLOT(del_protocol()));

    _layout = new QVBoxLayout();
    _layout->addLayout(hori_layout);
    _layout->addStretch(1);

    setLayout(_layout);
}

ProtocolDock::~ProtocolDock()
{
}

void ProtocolDock::paintEvent(QPaintEvent *)
{
     QStyleOption opt;
     opt.init(this);
     QPainter p(this);
     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ProtocolDock::add_protocol()
{
    if (_session.get_device()->mode != LOGIC) {
        QMessageBox msg(this);
        msg.setText("Protocol Analyzer");
        msg.setInformativeText("Protocol Analyzer is only valid in Digital Mode!");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setIcon(QMessageBox::Warning);
        msg.exec();
    } else {
        pv::decoder::DemoConfig dlg(this, _session.get_device(), _protocol_combobox->currentIndex());
        if (dlg.exec()) {
            std::list <int > _sel_probes = dlg.get_sel_probes();
            QMap <QString, QVariant>& _options = dlg.get_options();
            QMap <QString, int> _options_index = dlg.get_options_index();

            QPushButton *_del_button = new QPushButton(this);
            QPushButton *_set_button = new QPushButton(this);
            _del_button->setFlat(true);
            _del_button->setIcon(QIcon::fromTheme("protocol",
                                 QIcon(":/icons/del.png")));
            _set_button->setFlat(true);
            _set_button->setIcon(QIcon::fromTheme("protocol",
                                 QIcon(":/icons/set.png")));
            QLabel *_protocol_label = new QLabel(this);

            _del_button->setCheckable(true);
            _protocol_label->setText(_protocol_combobox->currentText());

            connect(_del_button, SIGNAL(clicked()),
                    this, SLOT(del_protocol()));
            connect(_set_button, SIGNAL(clicked()),
                    this, SLOT(rst_protocol()));

            _del_button_list.push_back(_del_button);
            _set_button_list.push_back(_set_button);
            _protocol_label_list.push_back(_protocol_label);
            _protocol_index_list.push_back(_protocol_combobox->currentIndex());

            QHBoxLayout *hori_layout = new QHBoxLayout();
            hori_layout->addWidget(_set_button);
            hori_layout->addWidget(_del_button);
            hori_layout->addWidget(_protocol_label);
            hori_layout->addStretch(1);
            _hori_layout_list.push_back(hori_layout);
            _layout->insertLayout(_del_button_list.size(), hori_layout);

            _session.add_protocol_analyzer(_protocol_combobox->currentIndex(), _sel_probes, _options, _options_index);
        }
    }
}

void ProtocolDock::rst_protocol()
{
    int rst_index = 0;
    for (QVector <QPushButton *>::const_iterator i = _set_button_list.begin();
         i != _set_button_list.end(); i++) {
        QPushButton *button = qobject_cast<QPushButton *>(sender());
        if ((*i) == button) {
            pv::decoder::DemoConfig dlg(this, _session.get_device(), _protocol_index_list.at(rst_index));
            dlg.set_config(_session.get_decode_probes(rst_index), _session.get_decode_options_index(rst_index));
            if (dlg.exec()) {
                std::list <int > _sel_probes = dlg.get_sel_probes();
                QMap <QString, QVariant>& _options = dlg.get_options();
                QMap <QString, int> _options_index = dlg.get_options_index();

                _session.rst_protocol_analyzer(rst_index, _sel_probes, _options, _options_index);
            }
            break;
        }
        rst_index++;
    }
}

void ProtocolDock::del_protocol()
{
    if (_del_all_button->isChecked()) {
        _del_all_button->setChecked(false);
        if (_hori_layout_list.size() > 0) {
            int del_index = 0;
            for (QVector <QHBoxLayout *>::const_iterator i = _hori_layout_list.begin();
                 i != _hori_layout_list.end(); i++) {
                _layout->removeItem((*i));
                delete (*i);
                delete _del_button_list.at(del_index);
                delete _set_button_list.at(del_index);
                delete _protocol_label_list.at(del_index);

                _session.del_protocol_analyzer(0);
                del_index++;
            }
            _hori_layout_list.clear();
            _del_button_list.clear();
            _set_button_list.clear();
            _protocol_label_list.clear();
            _protocol_index_list.clear();
        } else {
            QMessageBox msg(this);
            msg.setText("Protocol Analyzer");
            msg.setInformativeText("No Protocol Analyzer to delete!");
            msg.setStandardButtons(QMessageBox::Ok);
            msg.setIcon(QMessageBox::Warning);
            msg.exec();
        }
    } else {
       int del_index = 0;
       for (QVector <QPushButton *>::const_iterator i = _del_button_list.begin();
            i != _del_button_list.end(); i++) {
           if ((*i)->isChecked()) {
               _layout->removeItem(_hori_layout_list.at(del_index));

               delete _hori_layout_list.at(del_index);
               delete _del_button_list.at(del_index);
               delete _set_button_list.at(del_index);
               delete _protocol_label_list.at(del_index);

               _hori_layout_list.remove(del_index);
               _del_button_list.remove(del_index);
               _set_button_list.remove(del_index);
               _protocol_label_list.remove(del_index);
               _protocol_index_list.remove(del_index);

               _session.del_protocol_analyzer(del_index);

               break;
           }
           del_index++;
       }
    }
}

void ProtocolDock::del_all_protocol()
{
    if (_hori_layout_list.size() > 0) {
        int del_index = 0;
        for (QVector <QHBoxLayout *>::const_iterator i = _hori_layout_list.begin();
             i != _hori_layout_list.end(); i++) {
            _layout->removeItem((*i));
            delete (*i);
            delete _del_button_list.at(del_index);
            delete _set_button_list.at(del_index);
            delete _protocol_label_list.at(del_index);

            _session.del_protocol_analyzer(0);
            del_index++;
        }
        _hori_layout_list.clear();
        _del_button_list.clear();
        _set_button_list.clear();
        _protocol_label_list.clear();
        _protocol_index_list.clear();
    }
}

} // namespace dock
} // namespace pv
