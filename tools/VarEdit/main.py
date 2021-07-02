import sys
from typing import Final

from PyQt5.QtGui import QStandardItemModel, QStandardItem, QFont, QColor
from PyQt5.QtWidgets import QWidget, QApplication, QTreeView, QMainWindow

VERSION_MAJOR: Final = 0
VERSION_MINOR: Final = 0
VERSION_PATCH: Final = 0
VERSION = f"{VERSION_MAJOR}.{VERSION_MINOR}.{VERSION_PATCH}"

WINDOW_WIDTH: Final = 640
WINDOW_HEIGHT: Final = 480


class StandardItem(QStandardItem):
    def __init__(self, txt='', font_size=12, set_bold=False, color=QColor(0, 0, 0)):
        super().__init__()

        fnt = QFont('Open Sans', font_size)
        fnt.setBold(set_bold)

        self.setEditable(False)
        self.setForeground(color)
        self.setFont(fnt)
        self.setText(txt)


class Window(QMainWindow):
    def __init__(self, parent=None):
        super(Window, self).__init__(parent)
        self.resize(WINDOW_WIDTH, WINDOW_HEIGHT)
        self.setWindowTitle(f"VarEdit - V{VERSION}")
        self.init_gui()

    def init_gui(self):
        self.setGeometry(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)

        self.baseView = QTreeView()
        self.baseView.setHeaderHidden(True)

        self.treeModel = QStandardItemModel()
        self.rootNote = self.treeModel.invisibleRootItem()

        self.testVarGroupA = StandardItem("TestVarGroupA", 16, set_bold=True)

        itemA = StandardItem('Item A', 12, color=QColor(255, 0, 255))
        itemB = StandardItem('Item B', 12, color=QColor(255, 0, 255))
        itemC = StandardItem('Item c', 12, color=QColor(255, 0, 255))

        self.testVarGroupA.appendRow(itemA)
        self.testVarGroupA.appendRow(itemB)
        self.testVarGroupA.appendRow(itemC)

        self.testVarGroupB = StandardItem("TestVarGroupB", 16, set_bold=True)

        itemA = StandardItem('Item A', 12, color=QColor(255, 0, 255))
        itemB = StandardItem('Item B', 12, color=QColor(255, 0, 255))
        itemC = StandardItem('Item c', 12, color=QColor(255, 0, 255))

        self.testVarGroupB.appendRow(itemA)
        self.testVarGroupB.appendRow(itemB)
        self.testVarGroupB.appendRow(itemC)

        self.rootNote.appendRow(self.testVarGroupA)
        self.rootNote.appendRow(self.testVarGroupB)

        self.baseView.setModel(self.treeModel)
        self.baseView.expandAll()
        self.baseView.doubleClicked.connect(self.get_value)

        self.setCentralWidget(self.baseView)

    def get_value(self, ev):
        print(ev)


if __name__ == '__main__':
    app = QApplication(sys.argv)

    window = Window()
    window.show()

    sys.exit(app.exec_())
