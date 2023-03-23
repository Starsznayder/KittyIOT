#ifndef TREEHEADERBASE_H
#define TREEHEADERBASE_H
#include <QHeaderView>
#include <QPainter>
#include <QCheckBox>

namespace kitty{
namespace gui{

class TreeHeader : public QHeaderView
{
    Q_OBJECT
public:

  TreeHeader(Qt::Orientation orientation, QWidget * parent = 0) : QHeaderView(orientation, parent)
  {
      box = new QCheckBox(this);
      box->setGeometry(23,3,13,13);
  }

  void setChecked(bool arg)
  {
      box->setChecked(arg);
  }

  QCheckBox* box;

};

}
}

#endif // TREEHEADERBASE_H
