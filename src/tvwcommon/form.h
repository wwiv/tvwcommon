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

#ifndef INCLUDED_TVCOMMON_FORM_H
#define INCLUDED_TVCOMMON_FORM_H

#define Uses_TListViewer
#define Uses_TDialog
#define Uses_TRect
#define Uses_TScroller
#define Uses_TScrollBar

#include "tvision/tv.h"
#include <optional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>


class TFormColumn {
public:
  enum class LabelPosition { left, above };
  struct Options {
    int x{ 0 };
    int y{ 0 };
    int colspan{ 1 };
    int pad{ 2 };
    int maxLabelWidth{ 10 };
    int maxControlWidth{ 20 };
    LabelPosition labelPos{ LabelPosition::left };
  };

  TFormColumn(Options options);
  TFormColumn(int x, int y, int pad, int labelWidth, int controlWidth,
              LabelPosition labelPos);
  TFormColumn(int labelWidth, int controlWidth, LabelPosition labelPos)
      : TFormColumn(0, 0, 2, labelWidth, controlWidth, labelPos) {}
  ~TFormColumn();

  bool add(const std::string& labelText, TView* control);
  bool add(TFormColumn* column);

  int width() const;
  int height() const;
  void set_x(int x) { x_ = x; }
  int x() const { return x_; }
  int y() const { return y_; }
  int moveX(int r) { x_ += r; return x_; }

  void set_ypad(int y) { ypad_ = y; }
  void set_num(int num) { num_ = num; }

  void updateLabelWidths() const;
  TGroup* insertTo(TGroup* dialog);
  TGroup* insertTo(TGroup* dialog, int x, int y);
  bool selectFirstControl();

  const std::vector<TFormColumn*>& sub_columns() const { return sub_columns_; }

private:
  struct Item {
    std::string labelText;
    TView *control{ nullptr };
  };

  TGroup* insertSubFormLeft(TGroup* dialog, TFormColumn* col, int x, int y);
  TGroup* insertLabelLeftTo(TGroup* dialog, int x, int y);
  TGroup* insertLabelBelowTo(TGroup* dialog, int x, int y);

  // Column number
  int num_{ 0 }; 
  int x_{ 0 };
  int y_{ 0 };
  int colspan_{ 1 };
  int pad_{ 0 };
  int ypad_{ 1 };
  // Kunda hacky, but want ::width() to be const.
  mutable int labelWidth_{ 0 };
  mutable int controlWidth_{ 0 };
  LabelPosition labelPos_{ LabelPosition::left };
  std::vector<Item> items_;
  std::vector<TFormColumn*> sub_columns_;
};

class TForm {
public:
  TForm(TFormColumn* c1, TFormColumn* c2) { add(c1); add(c2); }
  TForm(TFormColumn* c) { add(c); }
  TForm() = default;
  ~TForm() = default;
  void add(TFormColumn* c);
  void addButton(const std::string& label, int command, int flags);
  void addOKButton();
  void addCancelButton();
  std::optional<TDialog*> createDialog(const std::string& title);

private:

  struct Button {
    std::string label;
    int command;
    int flags;
  };

  std::vector<TFormColumn*> cols_;
  std::vector<Button> buttons_;
};


#endif

