#include "keyitem.h"
#include <qredisclient/utils/text.h>
#include <QCoreApplication>
#include <QMenu>
#include <QMessageBox>

#include "connections-tree/model.h"
#include "connections-tree/utils.h"
#include "connections-tree/items/abstractnamespaceitem.h"

using namespace ConnectionsTree;

QSharedPointer<AbstractNamespaceItem> parentTreeItemToNs(QWeakPointer<TreeItem> p)
{
     auto parentNs = p.toStrongRef();

     if (!parentNs || !parentNs.staticCast<AbstractNamespaceItem>())
         return QSharedPointer<AbstractNamespaceItem>();

     return parentNs.staticCast<AbstractNamespaceItem>();
}

KeyItem::KeyItem(const QByteArray& fullPath,                 
                 QWeakPointer<TreeItem> parent, Model& model)
    : TreeItem(model),
      m_fullPath(fullPath),
      m_parent(parent),
      m_removed(false) {

}

QString KeyItem::getDisplayName() const {
  return printableString(getFullPath(), true);
}

QByteArray KeyItem::getName() const { return getFullPath(); }

QList<QSharedPointer<TreeItem>> KeyItem::getAllChilds() const {
  return QList<QSharedPointer<TreeItem>>();
}

bool KeyItem::supportChildItems() const { return false; }

uint KeyItem::childCount(bool) const { return 0u; }

QSharedPointer<TreeItem> KeyItem::child(uint) const {
  return QSharedPointer<TreeItem>();
}

QWeakPointer<TreeItem> KeyItem::parent() const { return m_parent; }

bool KeyItem::isEnabled() const {
  if (!m_removed && m_parent) {
    return m_parent.toStrongRef()->isEnabled();
  } else {
    return m_removed == false;
  }
}

QByteArray KeyItem::getFullPath() const { return m_fullPath; }

int KeyItem::getDbIndex() const {
    auto parentNs = parentTreeItemToNs(m_parent);

    if (!parentNs) {
        return -1;
    }

    return parentNs->getDbIndex();
}

void KeyItem::setRemoved() {
  m_removed = true;

  emit m_model.itemChanged(getSelf());
}

QHash<QString, std::function<void ()> > KeyItem::eventHandlers()
{
    auto events = TreeItem::eventHandlers();

    events.insert("click", [this]() {
        if (!isEnabled()) return;

        auto parentNs = parentTreeItemToNs(m_parent);

        if (!parentNs || !parentNs->operations())
            return;

      parentNs->operations()->openKeyTab(*this, false);
    });

    events.insert("mid-click", [this]() {
      if (!isEnabled()) return;

      auto parentNs = parentTreeItemToNs(m_parent);

      if (!parentNs || !parentNs->operations())
          return;

      parentNs->operations()->openKeyTab(*this, true);
    });

    events.insert("delete", [this]() {
      confirmAction(nullptr,
                    QCoreApplication::translate(
                        "RDM", "Do you really want to delete this key?"),
                    [this]() {

                      auto parentNs = parentTreeItemToNs(m_parent);

                      if (!parentNs || !parentNs->operations())
                          return;

                      parentNs->operations()->deleteDbKey(*this, [](const QString& error) {
                        QMessageBox::warning(
                            nullptr,
                            QCoreApplication::translate("RDM", "Key error"),
                            error);
                      });
                    });
    });
    return events;
}
