#include "myword.h"
#include <QFile>
#include <QDir>
#include <QMdiArea>
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QComboBox>
#include <QFontComboBox>
#include <QApplication>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QSignalMapper>
#include <QTextCharFormat>
#include <QFileDialog>
#include <QColorDialog>
#include <QDebug>
#include "childwnd.h"

const int c_iTimeout = 2000;

MyWord::MyWord(QWidget *parent)
    : QMainWindow(parent)
{
    _mdiArea = new QMdiArea;
    _mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setCentralWidget(_mdiArea);
    connect(_mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(updateActiveWndAndActions()));
    
    // 创建信号映射器
    _wndMapper = new QSignalMapper(this);
    // 映射器重新发送信号,根据信号设置活动窗口
    connect(_wndMapper, SIGNAL(mapped(QWidget *)), this,  SLOT(setActiveSubWindow(QWidget *)));
    
    createActions();
    createMenu();
    createToolBar();
    createStatusBar();
    updateActiveWndAndActions();
    
    this->move(200, 150);
    this->resize(800, 500);
    this->setWindowTitle(tr("Word多文档"));
}

MyWord::~MyWord()
{
}

void MyWord::createActions()
{
    _newAct = new QAction(QIcon(":/bmp/images/bmp/new.bmp"), tr("新建(&N)"));
    _newAct->setShortcut(QKeySequence::New);
    _newAct->setToolTip(tr("新建"));
    _newAct->setStatusTip(tr("创建一个新文档"));
    connect(_newAct, SIGNAL(triggered()), SLOT(fileNew()));
    
    _openAct = new QAction(QIcon(":/bmp/images/bmp/open.bmp"), tr("打开(&O)"));
    _openAct->setShortcut(QKeySequence::Open);
    _openAct->setToolTip(tr("打开"));
    _openAct->setStatusTip(tr("打开已存在的文档"));
    connect(_openAct, SIGNAL(triggered()), SLOT(fileOpen()));
    
    _saveAct = new QAction(QIcon(":/bmp/images/bmp/save.bmp"), tr("保存(&S)"));
    _saveAct->setShortcut(QKeySequence::Save);
    _saveAct->setToolTip(tr("保存"));
    _saveAct->setStatusTip(tr("将当前文档存盘"));
    connect(_saveAct, SIGNAL(triggered()), SLOT(fileSave()));
    
    _saveAsAct = new QAction(QIcon(":/bmp/images/bmp/SaveAs.bmp"), tr("另存为(&A)..."));
    _saveAsAct->setShortcut(QKeySequence::SaveAs);
    _saveAsAct->setStatusTip(tr("以一个新名字保存文档"));
    connect(_saveAsAct, SIGNAL(triggered()), SLOT(fileSaveAs()));
    
    _printAct = new QAction(QIcon(":/bmp/images/bmp/print.bmp"), tr("打印(&P)"));
    _printAct->setShortcut(QKeySequence::Print);
    _printAct->setToolTip(tr("打印"));
    _printAct->setStatusTip(tr("打印文档"));
    connect(_printAct, SIGNAL(triggered()), SLOT(filePrint()));

    
    _printPreviewAct = new QAction(QIcon(":/bmp/images/bmp/printPreview.bmp"), tr("打印预览..."));
    _printPreviewAct->setStatusTip(tr("预览打印效果"));
    connect(_printPreviewAct, SIGNAL(triggered()), SLOT(filePrintPreview()));
    
    _exitAct = new QAction(QIcon(":/bmp/images/bmp/exit.bmp"), tr("退出(&Q)"));
    _exitAct->setShortcut(QKeySequence::Quit);
    _exitAct->setStatusTip(tr("退出应用程序"));
    connect(_exitAct, SIGNAL(triggered()), SLOT(closeAllWindows()));
    
    _undoAct = new QAction(QIcon(":/bmp/images/bmp/undo.bmp"), tr("撤销(&U)"));
    _undoAct->setShortcut(QKeySequence::Undo);
    _undoAct->setToolTip("撤销");
    _undoAct->setStatusTip(tr("撤销当前操作"));
    connect(_undoAct, SIGNAL(triggered()), SLOT(undo()));
    
    _redoAct = new QAction(QIcon(":/bmp/images/bmp/redo.bmp"), tr("重做(&R)"));
    _redoAct->setShortcut(QKeySequence::Redo);
    _redoAct->setToolTip(tr("重做"));
    _redoAct->setStatusTip(tr("恢复之前操作"));
    connect(_redoAct, SIGNAL(triggered()), SLOT(redo()));
    
    _cutAct = new QAction(QIcon(":/bmp/images/bmp/cut.bmp"), tr("剪切(&T)"));
    _cutAct->setShortcut(QKeySequence::Cut);
    _cutAct->setToolTip(tr("剪切"));
    _cutAct->setStatusTip(tr("从文档中裁剪所选内容,并将其放入剪切板"));
    connect(_cutAct, SIGNAL(triggered()), SLOT(cut()));
    
    _copyAct = new QAction(QIcon(":/bmp/images/bmp/copy.bmp"), tr("复制(&C)"));
    _copyAct->setShortcut(QKeySequence::Copy);
    _copyAct->setToolTip(tr("复制"));
    _copyAct->setStatusTip(tr("复制所选内容,并将其放入剪切板"));
    connect(_copyAct, SIGNAL(triggered()), SLOT(copy()));
    
    _pasteAct = new QAction(QIcon(":/bmp/images/bmp/paste.bmp"), tr("粘贴(&P)"));
    _pasteAct->setShortcut(QKeySequence::Paste);
    _pasteAct->setToolTip(tr("粘贴"));
    _pasteAct->setStatusTip(tr("将剪切板的内容粘贴到文档中"));
    connect(_pasteAct, SIGNAL(triggered()), SLOT(paste()));
    
    _boldAct = new QAction(QIcon(":/bmp/images/bmp/bold.bmp"), tr("加粗(&B)"));
    _boldAct->setCheckable(true);
    _boldAct->setShortcut(Qt::CTRL + Qt::Key_B);    // QKeySequence::Bold
    _boldAct->setToolTip(tr("加粗"));
    _boldAct->setStatusTip(tr("将所选文字加粗"));
    QFont bold;
    bold.setBold(true);
    _boldAct->setFont(bold);
    connect(_boldAct, SIGNAL(triggered()), SLOT(textBold()));
    
    _italicAct = new QAction(QIcon(":/bmp/images/bmp/Italics.bmp"), tr("倾斜(&I)"));
    _italicAct->setShortcut(Qt::CTRL + Qt::Key_I);
    _italicAct->setCheckable(true);
    _italicAct->setToolTip(tr("倾斜"));
    _italicAct->setStatusTip(tr("将所选文字用斜体显示"));
    QFont italic;
    italic.setItalic(true);
    _italicAct->setFont(italic);
    connect(_italicAct, SIGNAL(triggered()), SLOT(textItalic()));
    
    _underlineAct = new QAction(QIcon(":/bmp/images/bmp/underline.bmp"), tr("下划线(&U)"));
    _underlineAct->setCheckable(true);
    _underlineAct->setShortcut(Qt::CTRL + Qt::Key_U);
    _underlineAct->setToolTip(tr("下划线"));
    _underlineAct->setStatusTip(tr("为所选文字添加下划线"));
    QFont underline;
    underline.setUnderline(true);
    _underlineAct->setFont(underline);
    connect(_underlineAct, SIGNAL(triggered()), SLOT(textUnderline()));
    
    QActionGroup *actGrp = new QActionGroup(this);
    connect(actGrp, SIGNAL(triggered(QAction *)), SLOT(textAlign(QAction *)));
    _leftAlignAct = new QAction(QIcon(":/images/bmp/AlignLeft.bmp"), tr("左对齐(&L)"), actGrp);
    _centerAct = new QAction(QIcon(":/images/bmp/CenterHorizontally.bmp"), tr("居中(&E)"), actGrp);
    _rightAlignAct = new QAction(QIcon(":/images/bmp/AlignRight.bmp"), tr("右对齐(&R)"), actGrp);
    _justifyAct = new QAction(QIcon(":/images/bmp/Justified.bmp"), tr("两端对齐"), actGrp);
    
    _leftAlignAct->setShortcut(Qt::CTRL + Qt::Key_L);
    _leftAlignAct->setCheckable(true);
    _leftAlignAct->setToolTip(tr("左对齐"));
    
    _centerAct->setShortcut(Qt::CTRL + Qt::Key_E);
    _centerAct->setCheckable(true);
    _centerAct->setToolTip(tr("居中"));
    
    _rightAlignAct->setShortcut(Qt::CTRL + Qt::Key_R);
    _rightAlignAct->setCheckable(true);
    _rightAlignAct->setToolTip(tr("右对齐"));
    
    _justifyAct->setShortcut(Qt::CTRL + Qt::Key_J);
    _justifyAct->setCheckable(true);
    _justifyAct->setToolTip(tr("两端对齐"));
    
    QPixmap pix(16, 16);
    pix.fill(Qt::red);
    _colorAct = new QAction(pix, tr("字体颜色(&C)..."), this);
    _colorAct->setToolTip(tr("字体颜色"));
    _colorAct->setStatusTip(tr("设置文字颜色"));
    connect(_colorAct, SIGNAL(triggered()), SLOT(textColor()));

    _insertAct= new QAction(tr("插入"));
    _insertAct->setToolTip(tr("插入"));
    connect(_insertAct, SIGNAL(triggered()), SLOT(insert()));

    _closeAct = new QAction(tr("关闭(&O)"));
    _closeAct->setStatusTip(tr("关闭活动文档子窗体"));
    connect(_closeAct, SIGNAL(triggered()), _mdiArea, SLOT(closeActiveSubWindow()));
    
    _closeAllAct = new QAction(tr("关闭所有(&A)"));
    connect(_closeAllAct, SIGNAL(triggered()), SLOT(closeAllSubWindows()));
    
    _titleAct = new QAction(tr("平铺(&T)"));
    _titleAct->setStatusTip(tr("平铺子窗口"));
    connect(_titleAct, SIGNAL(triggered()), _mdiArea, SLOT(tileSubWindows()));
    
    _cascadeAct = new QAction(tr("层叠(&C)"));
    _cascadeAct->setStatusTip(tr("层叠子窗口"));
    connect(_cascadeAct, SIGNAL(triggered()), _mdiArea, SLOT(cascadeSubWindows()));
    
    _nextAct = new QAction(tr("下一个(&X)"));
    _nextAct->setShortcut(QKeySequence::NextChild);
    _nextAct->setStatusTip(tr("移动焦点到下一个子窗体"));
    connect(_nextAct, SIGNAL(triggered()), _mdiArea, SLOT(activateNextSubWindow()));
    
    _previousAct = new QAction(tr("前一个(&V)"));
    _previousAct->setShortcut(QKeySequence::PreviousChild);
    connect(_previousAct, SIGNAL(triggered()), _mdiArea, SLOT(activatePreviousSubWindow()));
    
    _separatorAct = new QAction(this);
    _separatorAct->setSeparator(true);
    
    _aboutAct = new QAction(tr("关于(&A)"));
    _aboutAct->setStatusTip(tr("关于..."));
    connect(_aboutAct, SIGNAL(triggered()), SLOT(about()));
    
    _aboutQtAct = new QAction(tr("关于 Qt(&Q)"));
    _aboutQtAct->setStatusTip(tr("关于Qt"));
    connect(_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MyWord::createToolBar()
{
    _fileToolBar = this->addToolBar(tr("文件工具栏"));
    _fileToolBar->addAction(_newAct);
    _fileToolBar->addAction(_openAct);
    _fileToolBar->addSeparator();
    _fileToolBar->addAction(_saveAct);
    _fileToolBar->addAction(_saveAsAct);
    _fileToolBar->addSeparator();
    _fileToolBar->addAction(_printAct);
    _fileToolBar->addAction(_printPreviewAct);
    _fileToolBar->addSeparator();
    _fileToolBar->addAction(_exitAct);
    
    _editToolBar = this->addToolBar(tr("编辑工具栏"));
    _editToolBar->addAction(_undoAct);
    _editToolBar->addAction(_redoAct);
    _editToolBar->addSeparator();
    _editToolBar->addAction(_cutAct);
    _editToolBar->addAction(_copyAct);
    _editToolBar->addAction(_pasteAct);
    _editToolBar->addAction(_insertAct);
    
    _formatToolBar = this->addToolBar(tr("格式工具栏"));
    _formatToolBar->addAction(_boldAct);
    _formatToolBar->addAction(_italicAct);
    _formatToolBar->addAction(_underlineAct);
    _formatToolBar->addAction(_leftAlignAct);
    _formatToolBar->addAction(_centerAct);
    _formatToolBar->addAction(_rightAlignAct);
    _formatToolBar->addAction(_justifyAct);
    _formatToolBar->addAction(_colorAct);
    
    // 组合选择栏,可以使工具条分多行显示
    this->addToolBarBreak(Qt::TopToolBarArea);
    _comboToolBar = this->addToolBar(tr("组合选择"));
    _comboStyle  = new QComboBox();
    _comboToolBar->addWidget(_comboStyle);
    _comboStyle->addItem("标准");
    _comboStyle->addItem("项目符号 (●)");
    _comboStyle->addItem("项目符号 (○)");
    _comboStyle->addItem("项目符号 (■)");
    _comboStyle->addItem("项目符号 (1.2.3)");
    _comboStyle->addItem("项目符号 (a.b.c)");
    _comboStyle->addItem("项目符号 (A.B.C)");
    _comboStyle->addItem("项目符号 (ⅰ.ⅱ.ⅲ)");
    _comboStyle->addItem("项目符号 (Ⅰ.Ⅱ.Ⅲ)");
    _comboStyle->setStatusTip(tr("段落加标号或编号"));
    connect(_comboStyle, SIGNAL(activated(int)), SLOT(textStyle(int)));
    
    _comboFont = new QFontComboBox();
    _comboToolBar->addWidget(_comboFont);
    _comboFont->setStatusTip("更改字体");
    _comboFont->setEditable(false);
    connect(_comboFont, SIGNAL(activated(QString)), SLOT(textFamily(QString)));
    
    _comboSize = new QComboBox;
    _comboToolBar->addWidget(_comboSize);
    _comboSize->setEditable(true);
    _comboSize->setStatusTip("更改字号");
    
    QFontDatabase db;
    foreach (int size, db.standardSizes())
    {
        _comboSize->addItem(QString::number(size));
    }
    
    connect(_comboSize, SIGNAL(activated(QString)), SLOT(textSize(QString)));
    _comboSize->setCurrentIndex(_comboSize->findText(QString::number(QApplication::font().pointSize())));
}

void MyWord::createMenu()
{
    _fileMenu = this->menuBar()->addMenu(tr("文件(&F)"));
    //_fileMenu->setTearOffEnabled(true);
    _fileMenu->addAction(_newAct);
    _fileMenu->addAction(_openAct);
    _fileMenu->addSeparator();
    _fileMenu->addAction(_saveAct);
    _fileMenu->addAction(_saveAsAct);
    _fileMenu->addSeparator();
    _fileMenu->addAction(_printAct);
    _fileMenu->addAction(_printPreviewAct);
    _fileMenu->addSeparator();
    _fileMenu->addAction(_exitAct);



    _editMenu = this->menuBar()->addMenu(tr("编辑(&E)"));
    _editMenu->addAction(_undoAct);
    _editMenu->addAction(_redoAct);
    _editMenu->addSeparator();
    _editMenu->addAction(_cutAct);
    _editMenu->addAction(_copyAct);
    _editMenu->addAction(_pasteAct);
    
    _formatMenu = this->menuBar()->addMenu(tr("格式(&O)"));
    _fontMenu = _formatMenu->addMenu(tr("字体(&D)"));
    _fontMenu->addAction(_boldAct);
    _fontMenu->addAction(_italicAct);
    _fontMenu->addAction(_underlineAct);
    
    _alignMenu = _formatMenu->addMenu((tr("段落")));
    _alignMenu->addAction(_leftAlignAct);
    _alignMenu->addAction(_centerAct);
    _alignMenu->addAction(_rightAlignAct);
    _alignMenu->addAction(_justifyAct);
    _formatMenu->addAction(_colorAct);
    
    _windowMenu = this->menuBar()->addMenu(tr("窗口(&W)"));
    updateWindowMenu();
    connect(_windowMenu, SIGNAL(aboutToShow()), SLOT(updateWindowMenu()));
    this->menuBar()->addSeparator();
    
    _helpMenu = this->menuBar()->addMenu(tr("帮助(&H)"));
    _helpMenu->addAction(_aboutAct);
    _helpMenu->addSeparator();
    _helpMenu->addAction(_aboutQtAct);
}

void MyWord::createStatusBar()
{
    this->statusBar()->showMessage(tr("就绪"));
}

void MyWord::enabledText(bool bEnable)
{
    _boldAct->setEnabled(bEnable);
    _italicAct->setEnabled(bEnable);
    _underlineAct->setEnabled(bEnable);
    _leftAlignAct->setEnabled(bEnable);
    _centerAct->setEnabled(bEnable);
    _rightAlignAct->setEnabled(bEnable);
    _justifyAct->setEnabled(bEnable);
    _colorAct->setEnabled(bEnable);
}

void MyWord::fileNew()
{
    ChildWnd *child = createChildWnd();
    if (nullptr != child)
    {
        child->newFile();
        child->show();
        enabledText(true);
    }
}

void MyWord::fileOpen()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开"), QString(), tr("HTML 文档(*.htm *.html);;所有文件(*.*)"));
    if (!fileName.isEmpty())
    {
        QMdiSubWindow *existing = findChildWnd(fileName);
        if (nullptr == existing)
        {
            ChildWnd *child = createChildWnd();
            if (nullptr != child)
            {
                if (child->loadFile(fileName))
                {
                    child->show();
                    enabledText(true);
                    this->statusBar()->showMessage(tr("文件已载入"), c_iTimeout);
                }
                else
                {
                    child->close();
                }
            }
        }
        else
        {
            _mdiArea->setActiveSubWindow(existing);
        }
    }
}

void MyWord::fileSave()
{
    if (nullptr != _activeChildWnd && _activeChildWnd->save())
    {
        this->statusBar()->showMessage(tr("保存成功"), c_iTimeout);
    }
}

void MyWord::fileSaveAs()
{
    if (nullptr != _activeChildWnd && _activeChildWnd->saveAs())
    {
        this->statusBar()->showMessage(tr("保存成功"), c_iTimeout);
    }
}

void MyWord::filePrint()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    
    if (_activeChildWnd->textCursor().hasSelection())
    {
        dlg->setOption(QAbstractPrintDialog::PrintSelection);
    }
    
    dlg->setWindowTitle(tr("打印文档"));
    if (dlg->exec() == QDialog::Accepted)
    {
        _activeChildWnd->print(&printer);
    }
    
    delete dlg;
    dlg = nullptr;
}

void MyWord::printPreview(QPrinter *printer)
{
    _activeChildWnd->print(printer);
}

void MyWord::filePrintPreview()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog dlg(&printer, this);
    connect(&dlg, SIGNAL(paintRequested(QPrinter *)), SLOT(printPreview(QPrinter *)));
    dlg.exec();
}

// 平铺子窗体
void MyWord::tileSubWindows()
{
    _mdiArea->setViewMode(QMdiArea::SubWindowView);
}

// 层叠子窗体
void MyWord::cascadeSubWindows()
{
    _mdiArea->setViewMode(QMdiArea::TabbedView);
}

void MyWord::activateNextSubWindow()
{
    QWidget *nextWgt = _mdiArea->nextInFocusChain();
    if (nullptr != nextWgt)
    {
        QMdiSubWindow *nextSubWnd = qobject_cast<QMdiSubWindow *>(nextWgt);
        _mdiArea->setActiveSubWindow(nextSubWnd);
    }
}

void MyWord::activatePreviousSubWindow()
{
    QWidget *prevWgt = _mdiArea->previousInFocusChain();
    if (nullptr != prevWgt)
    {
        QMdiSubWindow *prevSubWnd = qobject_cast<QMdiSubWindow *>(prevWgt);
        _mdiArea->setActiveSubWindow(prevSubWnd);
    }
}

void MyWord::closeActiveSubWindow()
{
    if (nullptr != _activeChildWnd)
    {
        _activeChildWnd->close();
    }
}

void MyWord::setActiveSubWindow(QWidget *wgt)
{
    if (nullptr != wgt)
    {
        _activeChildWnd = qobject_cast<ChildWnd *>(wgt);
        
        QMdiSubWindow *subWnd = qobject_cast<QMdiSubWindow *>(wgt);
        foreach (const auto &wnd, _mdiArea->subWindowList())
        {
            if (subWnd == wnd)
            {
                _mdiArea->setActiveSubWindow(wnd);
                break;
            }
            else
            {                
            }
        }
    }
}

void MyWord::undo()
{
    if (nullptr != _activeChildWnd)
    {
        _activeChildWnd->undo();
    }
}

void MyWord::redo()
{
    if (nullptr != _activeChildWnd)
    {
        _activeChildWnd->redo();
    }
}

void MyWord::cut()
{
    if (nullptr != _activeChildWnd)
    {
        _activeChildWnd->cut();
    }
}

void MyWord::copy()
{
    if (nullptr != _activeChildWnd)
    {
        _activeChildWnd->copy();
    }
}

void MyWord::paste()
{
    if (nullptr != _activeChildWnd)
    {
        _activeChildWnd->paste();
    }
}

void MyWord::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(_boldAct->isChecked() ? QFont::Bold : QFont::Normal);
    if (_activeChildWnd != nullptr)
    {
        _activeChildWnd->mergeFormatOnWordOrSelection(fmt);
    }
}

void MyWord::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(_italicAct->isChecked());
    if (nullptr != _activeChildWnd)
    {
        _activeChildWnd->mergeFormatOnWordOrSelection(fmt);
    }
}

void MyWord::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(_underlineAct->isChecked());
    if (nullptr != _activeChildWnd)
    {
        _activeChildWnd->mergeFormatOnWordOrSelection(fmt);
    }
}

void MyWord::textAlign(QAction *act)
{
    if (_activeChildWnd != nullptr)
    {
        if (act == _leftAlignAct)
        {
            _activeChildWnd->setAlign(ChildWnd::EAlignLeft);
        }
        else if (act == _centerAct)
        {
            _activeChildWnd->setAlign(ChildWnd::EAlignCenter);
        }
        else if (act == _rightAlignAct)
        {
            _activeChildWnd->setAlign(ChildWnd::EAlignRight);
        }
        else if (act == _justifyAct)
        {
            _activeChildWnd->setAlign(ChildWnd::EAlignJustify);
        }
    }
}



void MyWord::textColor()
{
    if (nullptr != _activeChildWnd)
    {
        QColor clr = QColorDialog::getColor(_activeChildWnd->textColor(), this);
        if (clr.isValid())
        {
            QTextCharFormat fmt;
            fmt.setForeground(clr);
            _activeChildWnd->mergeFormatOnWordOrSelection(fmt);
            //colorChanged(clr);
        }
    }
}

void MyWord::about()
{
    QMessageBox::about(this, tr("关于"), tr("这是一个基于Qt实现文字处理的软件,具备类似微软Office Word的功能"));
}

void MyWord::closeAllWindows()
{
    this->close();
}

void MyWord::updateActiveWndAndActions()
{
    bool hasChildWnd = false;
    _activeChildWnd = nullptr;
    
    if (nullptr != _mdiArea)
    {
        QMdiSubWindow *activeSubWnd = _mdiArea->activeSubWindow();        
        if (nullptr != activeSubWnd)
        {
            hasChildWnd = true;
            _activeChildWnd = qobject_cast<ChildWnd *>(activeSubWnd->widget());
        }
        else
        {
        }    
    }
    else
    {        
    }

    // 是否至少有一个子文档打开
    _undoAct->setEnabled(hasChildWnd);
    _redoAct->setEnabled(hasChildWnd);
    _saveAct->setEnabled(hasChildWnd);
    _saveAsAct->setEnabled(hasChildWnd);
    _printAct->setEnabled(hasChildWnd);
    _printPreviewAct->setEnabled(hasChildWnd);
    _pasteAct->setEnabled(hasChildWnd);
    _closeAct->setEnabled(hasChildWnd);
    _closeAllAct->setEnabled(hasChildWnd);
    _titleAct->setEnabled(hasChildWnd);
    _cascadeAct->setEnabled(hasChildWnd);
    _nextAct->setEnabled(hasChildWnd);
    _previousAct->setEnabled(hasChildWnd);
    _separatorAct->setVisible(hasChildWnd);

    _cutAct->setEnabled(hasChildWnd);
    _copyAct->setEnabled(hasChildWnd);
    _boldAct->setEnabled(hasChildWnd);
    _italicAct->setEnabled(hasChildWnd);
    _underlineAct->setEnabled(hasChildWnd);
    _leftAlignAct->setEnabled(hasChildWnd);
    _centerAct->setEnabled(hasChildWnd);
    _rightAlignAct->setEnabled(hasChildWnd);
    _justifyAct->setEnabled(hasChildWnd);
    _colorAct->setEnabled(hasChildWnd);
}

void MyWord::closeAllSubWindows()
{
    foreach (QMdiSubWindow *wnd, _mdiArea->subWindowList())
    {
        if (nullptr != wnd)
        {
            wnd->close();
        }
    }
}

void MyWord::updateWindowMenu()
{
    // 首先清空菜单,然后再添加各个菜单动作
    _windowMenu->clear();
    _windowMenu->addAction(_closeAct);
    _windowMenu->addAction(_closeAllAct);
    _windowMenu->addSeparator();
    _windowMenu->addAction(_titleAct);
    _windowMenu->addAction(_cascadeAct);
    _windowMenu->addSeparator();
    _windowMenu->addAction(_nextAct);
    _windowMenu->addAction(_previousAct);
    _windowMenu->addAction(_separatorAct);
    
    //如果有活动窗口,则显示分割条
    QList<QMdiSubWindow *> windows = _mdiArea->subWindowList();
    _separatorAct->setVisible(!windows.isEmpty());
    
    // 遍历各个子窗口,显示当前已打开的文档子窗口
    for (int i = 0; i < windows.size(); ++i)
    {
        ChildWnd *child = qobject_cast<ChildWnd *>(windows.at(i)->widget());
        QString text;
        if (i < 9)
        {
            text = tr("&%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
        }
        else
        {
            text = tr("%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
        }
        
        QAction *act = _windowMenu->addAction(text);
        act->setCheckable(true);
        
        if (child == _activeChildWnd)
        {
            act->setChecked(true);
        }
        else
        {
            act->setChecked(false);
        }
        
        // 关联动作的触发信号到信号映射器的map()槽,这个槽会发送mapped()信号
        connect(act, SIGNAL(triggered()), _wndMapper, SLOT(map()));
        // 将动作与相应的窗口部件进行映射,在发送mapped()信号时就会以这个窗口部件为参数
        _wndMapper->setMapping(act, windows.at(i));
    }
    
    if (windows.length() > 0)
    {
        enabledText(true);      // 使字体设置菜单可用        
    }
    else
    {
        enabledText(false);
    }
}

void MyWord::textStyle(int styleIndex)
{
    if (_activeChildWnd)
    {
        _activeChildWnd->setStyle(styleIndex);
    }
}

void MyWord::textFamily(const QString &fontFamily)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(fontFamily);
    if (nullptr != _activeChildWnd)
    {
        _activeChildWnd->mergeFormatOnWordOrSelection(fmt);
    }
}

void MyWord::textSize(const QString &pointSize)
{
    qreal fontSize = pointSize.toFloat();
    if (fontSize > 0 && nullptr != _activeChildWnd)
    {
        QTextCharFormat fmt;
        fmt.setFontPointSize(fontSize);
        _activeChildWnd->mergeFormatOnWordOrSelection(fmt);
    }
}

void MyWord::onEditActionChecked()
{
    if (nullptr != _activeChildWnd)
    {
        Qt::Alignment align = _activeChildWnd->alignment();
        if (Qt::AlignLeft & align)
        {
            _leftAlignAct->setChecked(true);
        }
        else if (Qt::AlignRight & align)
        {
            _rightAlignAct->setChecked(true);
        }
        else if (Qt::AlignJustify & align)
        {
            _justifyAct->setChecked(true);
        }
        else if (Qt::AlignHCenter & align)
        {
            _centerAct->setChecked(true);
        }
        
        const QTextCharFormat &textFormat = _activeChildWnd->textCursor().charFormat();
        if (textFormat.isValid())
        {
            if (QFont::Bold == textFormat.fontWeight())
            {
                _boldAct->setChecked(true);
            }
            else
            {
                _boldAct->setChecked(false);
            }
            if (textFormat.fontItalic())
            {
                _italicAct->setChecked(true);
            }
            else
            {
                _italicAct->setChecked(false);
            }
            if (textFormat.fontUnderline())
            {
                _underlineAct->setChecked(true);
            }
            else
            {
                _underlineAct->setChecked(false);
            }
            if (textFormat.fontOverline())
            {
                ;
            }
            else
            {
                ;
            }
            if (textFormat.fontStrikeOut())
            {
                ;
            }
            else
            {
                ;
            }
        }
    }
}

/* void MyWord::updateMenus()
{
    // 是否至少有一个子文档打开
    bool hasChildWnd = (_activeChildWnd != nullptr);
    _saveAct->setEnabled(hasChildWnd);
    _saveAsAct->setEnabled(hasChildWnd);
    _printAct->setEnabled(hasChildWnd);
    _printPreviewAct->setEnabled(hasChildWnd);
    _pasteAct->setEnabled(hasChildWnd);
    _closeAct->setEnabled(hasChildWnd);
    _closeAllAct->setEnabled(hasChildWnd);
    _titleAct->setEnabled(hasChildWnd);
    _cascadeAct->setEnabled(hasChildWnd);
    _nextAct->setEnabled(hasChildWnd);
    _previousAct->setEnabled(hasChildWnd);
    _separatorAct->setVisible(hasChildWnd);
    
    // 文档打开并且其中有内容被选中的情况
    bool hasSelection = (nullptr != _activeChildWnd && _activeChildWnd->textCursor().hasSelection());
    _cutAct->setEnabled(hasSelection);
    _copyAct->setEnabled(hasSelection);
    _boldAct->setEnabled(hasSelection);
    _italicAct->setEnabled(hasSelection);
    _underlineAct->setEnabled(hasSelection);
    _leftAlignAct->setEnabled(hasSelection);
    _centerAct->setEnabled(hasSelection);
    _rightAlignAct->setEnabled(hasSelection);
    _justifyAct->setEnabled(hasSelection);
    _colorAct->setEnabled(hasSelection);
} */

ChildWnd *MyWord::createChildWnd()
{
    ChildWnd *child = new ChildWnd;
    _mdiArea->addSubWindow(child);
    
    connect(child, SIGNAL(copyAvailable(bool)), _cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)), _copyAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(redoAvailable(bool)), _redoAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(undoAvailable(bool)), _undoAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(cursorPositionChanged()), this, SLOT(onEditActionChecked()));
    
    return child;
}

QMdiSubWindow *MyWord::findChildWnd(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();
    foreach (QMdiSubWindow *window, _mdiArea->subWindowList())
    {
        ChildWnd *child = qobject_cast<ChildWnd *>(window->widget());
        if (child->currentFile() == canonicalFilePath)
        {
            return window;
        }
    }
    
    return nullptr;
}

void MyWord::closeEvent(QCloseEvent *event)
{
    _mdiArea->closeAllSubWindows();
    if (_mdiArea->currentSubWindow())
    {
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

