/*
 * Copyright (C)2023 WWIV Software Services
 *
 * Licensed  under the  Apache License, Version  2.0 (the "License");
 * you may not use this  file  except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless  required  by  applicable  law  or agreed to  in  writing,
 * software  distributed  under  the  License  is  distributed on an
 * "AS IS"  BASIS, WITHOUT  WARRANTIES  OR  CONDITIONS OF ANY  KIND,
 * either  express  or implied.  See  the  License for  the specific
 * language governing permissions and limitations under the License.
 */
#define Uses_MsgBox
#define Uses_TApplication
#define Uses_TButton
#define Uses_TChDirDialog
#define Uses_TCheckBoxes
#define Uses_TDeskTop
#define Uses_TDialog
#define Uses_TEditor
#define Uses_TEditWindow
#define Uses_TFileDialog
#define Uses_TFileEditor
#define Uses_THistory
#define Uses_TInputLine
#define Uses_TKeys
#define Uses_TLabel
#define Uses_TListViewer
#define Uses_TMenuBar
#define Uses_TMenuItem
#define Uses_TObject
#define Uses_TPoint
#define Uses_TProgram
#define Uses_TRect
#define Uses_TSItem
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TStatusLine
#define Uses_TSubMenu

#include "tvision/tv.h"
#include "form.h"
#include "fmt/format.h"
#include <sstream>
#include <string>

TFormColumn::TFormColumn(Options o)
    : TFormColumn(o.x, o.y, o.pad, o.maxLabelWidth, o.maxControlWidth,
                  o.labelPos) {}

TFormColumn::TFormColumn(int x, int y, int pad, int maxLabelWidth,
                         int maxControlWidth, TFormColumn::LabelPosition labelPos)
    : x_(x), y_(y), pad_(pad), labelWidth_(maxLabelWidth),
      controlWidth_(maxControlWidth), labelPos_(labelPos) {}

TFormColumn::~TFormColumn() = default;

bool TFormColumn::add(TFormColumn* col) {
  sub_columns_.emplace_back(col);
  return true;
}

bool TFormColumn::add(const std::string& labelText, TView* control) {
  Item item{};
  item.control = control;
  item.labelText = labelText;

  items_.emplace_back(item);
  return true;
}

int TFormColumn::width() const {
  updateLabelWidths();
  int width = 5;
  if (labelPos_ == TFormColumn::LabelPosition::left) {
    width = 1 + pad_ + labelWidth_ + pad_ + controlWidth_ + pad_;
  } else {
    width = 1 + (pad_ * 3) + std::max<int>(labelWidth_, controlWidth_);
  }

  for (const auto* sc : sub_columns_) {
    const auto sch = sc->width() + sc->x();
    if (sch > width) {
      width = sch;
    }
  }

  return width;
}

int TFormColumn::height() const {
  int height = 3;
  
  if (labelPos_ == TFormColumn::LabelPosition::left) {
    for (const auto& item : items_) {
      auto bounds = item.control->getBounds();
      height += (bounds.b.y - bounds.a.y) + ypad_;
    }
    return height;
  } 
  
  for (const auto& item : items_) {
    auto bounds = item.control->getBounds();
    height += (bounds.b.y - bounds.a.y) + 2;
  }
  return height;
}

void TFormColumn::updateLabelWidths() const{
  int labelWidth = 5;
  int controlWidth = 5;
  for (const auto& item : items_) {
    const auto bounds = item.control->getBounds();
    const auto c = bounds.b.x - bounds.a.x;
    labelWidth = std::max<int>(labelWidth, item.labelText.size());
    controlWidth = std::max<int>(controlWidth, c);
  }
  ++labelWidth;
  labelWidth_ = std::min<int>(labelWidth, labelWidth_);
  controlWidth_ = std::min<int>(controlWidth_, controlWidth);
}

TGroup* TFormColumn::insertSubFormLeft(TGroup* dialog, TFormColumn* c, int x, int y) {
  const auto height = c->height();
  const auto width = c->width();
  c->insertTo(dialog, x, y);
  return dialog;
}

TGroup* TFormColumn::insertLabelLeftTo(TGroup* dialog, int x, int y) {
  const int ctrlX = x + labelWidth_ + pad_;
  for (auto& item : items_) {
      const auto bounds = item.control->getBounds();
      const auto height = bounds.b.y - bounds.a.y;
      const auto existingWidth = bounds.b.x - bounds.a.x;
      // We want the smaller of control width or preferred width already set.
      const auto width = std::min<int>(controlWidth_, existingWidth);
      const TRect lr(x, y, ctrlX, y + 1);
      const TRect cr(ctrlX, y, ctrlX + width, y + height);
      item.control->setBounds(cr);
      dialog->insert(item.control);
      dialog->insert(new TLabel(lr, item.labelText, item.control));
      y += height + ypad_;
    }
  for (auto* c : sub_columns_) {
    const auto height = c->height();
    const auto width = c->width();

    insertSubFormLeft(dialog, c, c->x(), c->y());
  }
  return dialog;
}

TGroup* TFormColumn::insertLabelBelowTo(TGroup* dialog, int x, int y) {

  for (const auto& item : items_) {
    const auto bounds = item.control->getBounds();
    const auto height = bounds.b.y - bounds.a.y;
    TRect cr(x, y + 1, x + std::max<int>(labelWidth_, controlWidth_), y + height + 1);
    cr.move(0, 1);
    item.control->setBounds(cr);
    dialog->insert(item.control);

    TRect lr(x, y, x + std::max<int>(labelWidth_, controlWidth_), y + 1);
    dialog->insert(new TLabel(lr, item.labelText, item.control));
    y += height + 1 + 1;
  }
  return dialog;
}

TGroup* TFormColumn::insertTo(TGroup* dialog) {
  updateLabelWidths();
  return insertTo(dialog, x_ + pad_, y_ + 2);
}

TGroup* TFormColumn::insertTo(TGroup* dialog, int x, int y) {
  updateLabelWidths();
  if (labelPos_ == TFormColumn::LabelPosition::left) {
    return insertLabelLeftTo(dialog, x, y);
  }
  return insertLabelBelowTo(dialog, x, y);
}


bool TFormColumn::selectFirstControl() {
  if (items_.empty()) {
    return false;
  }
  items_.front().control->select();
  return true;
}

void TForm::add(TFormColumn* c) {
  c->set_num(cols_.size());
  cols_.emplace_back(c);
}

void TForm::addButton(const std::string& label, int command, int flags) {
  buttons_.emplace_back(label, command, flags);
}

void TForm::addOKButton() { addButton("~O~K", cmOK, bfDefault); }
void TForm::addCancelButton() { addButton("C~a~ncel", cmCancel, bfNormal); }

std::optional<TDialog*> TForm::createDialog(const std::string& title) {
  if (cols_.empty()) {
    return std::nullopt;
  }
  const int buttonHeight = 3;
  int width = 0; 
  int height = buttonHeight;
  for (const auto* c : cols_) {
    width += c->width();
    if (c->height() > height) {
      height = c->height();
    }
    for (const auto* sc : c->sub_columns()) {
      auto sch = c->height() + c->y();
      if (sch > height) {
        height = sch;
      }
    }
  }
  if (!buttons_.empty()) {
    height += buttonHeight;
  }
  TRect bounds(0, 0, width, height);
  TDialog* d = new TDialog(bounds, title);
  int x = 0;
  for (auto* c : cols_) {
    c->moveX(x);
    c->insertTo(d);
    x += c->x() + c->width();
  }

  // Add buttons
  constexpr int btnPadding = 4;
  constexpr int btnWidth = 10;
  const int y = d->getBounds().b.y - 3;
  int end_x = d->getBounds().b.x - 3;
  for (auto& b : buttons_) {
    TRect r(end_x - btnWidth, y, end_x, y + 2);
    d->insert(new TButton(r, b.label, b.command, b.flags));
    end_x -= (btnWidth + btnPadding);
  }

  cols_.front()->selectFirstControl();
  return d;
}

