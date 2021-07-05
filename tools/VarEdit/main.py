import os.path
import sys
import xml.etree.ElementTree as Et
from dataclasses import dataclass
from typing import Final
from os import walk

from PyQt5.QtGui import QStandardItemModel, QStandardItem, QFont, QColor, QIcon
from PyQt5.QtWidgets import QWidget, QApplication, QTreeView, QMainWindow, QHBoxLayout, QVBoxLayout, QListWidget, \
    QLineEdit, QAction, QPushButton

VERSION_MAJOR: Final = 0
VERSION_MINOR: Final = 0
VERSION_PATCH: Final = 0
VERSION = f"{VERSION_MAJOR}.{VERSION_MINOR}.{VERSION_PATCH}"

WINDOW_WIDTH: Final = 640
WINDOW_HEIGHT: Final = 480


@dataclass
class VarFile:
    path: str = ""
    name: str = ""


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
        self.current_file = None
        self.firstLoad = True
        self.files = []

        self.resize(WINDOW_WIDTH, WINDOW_HEIGHT)
        self.setWindowTitle(f"VarEdit - V{VERSION}")
        self.init_gui()

    def update_file_panel(self):
        if self.left_file_panel is None:
            return

        self.left_file_panel.addItems(list(map(lambda v: v.name, self.files)))
        self.left_file_panel.currentItemChanged.connect(self.left_file_panel_index_changed)

    def update_variable_panel(self):
        if not self.current_file or self.right_var_edit is None:
            return

        while self.root_node.rowCount() > 0:
            self.root_node.removeRow(0)

        tree = Et.parse(self.current_file.path)
        root = tree.getroot()

        def state_changed(index):
            print(index)

        root_item = StandardItem(f"{self.current_file.name}", 20, set_bold=True)

        for group in root:

            n = ""
            if "id" in group.attrib:
                n = f" {group.attrib['id']}"

            parent_node = StandardItem(f"{group.tag.capitalize()}{n}", 16, set_bold=True)

            for child in group:
                print(child)

                n = ""

                if "name" in child.attrib:
                    n = f" ({child.attrib['name']})"

                if "itemId" in child.attrib:
                    n += f" ({child.attrib['itemId']})"

                child_node = StandardItem(f"{child.tag.capitalize()}{n}", 14, set_bold=False)

                for item in child.attrib:
                    var_name_node = StandardItem(f"{item}", 12)
                    var_val_node = StandardItem(f"{child.attrib[item]}", 12)
                    var_val_node.setEditable(True)

                    # var_val_node.valueChanged

                    var_name_node.appendColumn([var_val_node])
                    child_node.appendRow(var_name_node)

                parent_node.appendRow(child_node)

            root_item.appendRow(parent_node)

        self.root_node.appendRow(root_item)

    def left_file_panel_index_changed(self, i):
        if self.firstLoad or i is None:
            self.firstLoad = False
            return

        for varfile in self.files:
            if varfile.name == i.text():
                self.current_file = varfile
                self.update_variable_panel()
                return

    def init_file_panel(self):
        layout = QListWidget()

        layout.setMaximumWidth(200)

        file_path = os.path.dirname(os.path.realpath(__file__))
        data_path = os.path.join(file_path, "..\\..\\resources\\data\\")

        for (_, _, filenames) in walk(data_path):
            for file in filenames:
                self.files.append(
                    VarFile(
                        path=f"{os.path.join(data_path, file)}",
                        name=f"{os.path.basename(file).replace('.xml', '')}"
                    ),
                )
            if len(self.files) > 0:
                self.current_file = self.files[0]
            break

        return layout

    def init_toolbar(self):
        self.toolbar = self.addToolBar("File")

        new = QAction(QIcon(), "Save", self)
        self.toolbar.addAction(new)

        self.search_container = QHBoxLayout()

        self.search_field = QLineEdit()
        self.search_button = QPushButton("search")

        self.search_container.addWidget(self.search_field)
        self.search_container.addWidget(self.search_button)

        self.container.addLayout(self.search_container)

    def init_gui(self):
        self.setGeometry(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)
        self.container = QVBoxLayout()
        self.container_widget = QWidget()

        self.init_toolbar()

        self.application_view = QHBoxLayout()
        self.application_view_widget = QWidget()
        self.application_view_widget.setLayout(self.application_view)

        self.left_file_panel = self.init_file_panel()
        self.right_var_edit = QVBoxLayout()

        self.application_view.addWidget(self.left_file_panel)
        self.application_view.addLayout(self.right_var_edit)

        self.base_view = QTreeView()
        self.right_var_edit.addWidget(self.base_view)

        self.base_view.setHeaderHidden(True)

        self.tree_model = QStandardItemModel()
        self.root_node = self.tree_model.invisibleRootItem()

        self.update_file_panel()
        self.update_variable_panel()

        self.base_view.setModel(self.tree_model)
        self.base_view.expandAll()
        self.base_view.doubleClicked.connect(self.get_value)

        self.container.addWidget(self.application_view_widget)

        self.container_widget.setLayout(self.container)
        self.setCentralWidget(self.container_widget)

    def get_value(self, ev):
        pass


if __name__ == '__main__':
    app = QApplication(sys.argv)

    window = Window()
    window.show()

    sys.exit(app.exec_())
