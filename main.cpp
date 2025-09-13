// main.cpp
#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget> // The best widget for swapping views
#include <map>           // To map page names to widgets

#include "Store.h"

// A "heavy" component for our example
QWidget* createHeavyPage(const QString& name) {
    QWidget* page = new QWidget();
    page->setStyleSheet("background-color: white; border: 1px solid #cccccc; border-radius: 5px;");
    QLabel* label = new QLabel(QString("This is the HEAVY layout for\n'%1'").arg(name));
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 16px;");
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->addWidget(label);
    return page;
}

// Our optimized sidebar
class Sidebar : public QWidget {
    Q_OBJECT
public:
    explicit Sidebar(QWidget* parent = nullptr) : QWidget(parent) {
        // --- 1. Create all heavy widgets UPFRONT ---
        m_stackedWidget = new QStackedWidget();

        // Use a map to easily find widgets by their state name
        m_pageMap["chatList"] = createHeavyPage("chatList");
        m_pageMap["filesSidebar"] = createHeavyPage("filesSidebar");
        m_pageMap["bugSidebar"] = createHeavyPage("bugSidebar");

        // Add them to the QStackedWidget
        for (auto const& [name, widget] : m_pageMap) {
            m_stackedWidget->addWidget(widget);
        }

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(new QLabel("Sidebar:"));
        layout->addWidget(m_stackedWidget);

        // Connect to the store's signal
        connect(&Store::instance(), &Store::stateChanged, this, &Sidebar::onStateChanged);

        // Set the initial view from the state
        onStateChanged();
    }

public slots:
    void onStateChanged() {
        const auto& newPageName = Store::instance().getState().sidebar.activePage;

        // --- OPTIMIZATION 1: Check if our data has actually changed ---
        if (newPageName != m_currentPage) {
            qDebug() << "Sidebar state CHANGED. Updating view to:" << QString::fromStdString(newPageName);
            m_currentPage = newPageName;

            // --- OPTIMIZATION 2: Don't rebuild, just switch the visible widget ---
            if (m_pageMap.count(m_currentPage)) {
                m_stackedWidget->setCurrentWidget(m_pageMap[m_currentPage]);
            }
        } else {
            qDebug() << "Sidebar state did NOT change. Ignoring signal.";
        }
    }

private:
    QStackedWidget* m_stackedWidget;
    std::map<std::string, QWidget*> m_pageMap;
    std::string m_currentPage;
};

// Main application window
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QMainWindow mainWindow;
    QWidget* centralWidget = new QWidget();
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    Sidebar* sidebar = new Sidebar();
    sidebar->setFixedWidth(250);
    sidebar->setStyleSheet("background-color: #f0f0f0;");

    // Control buttons
    QWidget* controls = new QWidget();
    QVBoxLayout* controlsLayout = new QVBoxLayout(controls);

    QPushButton* toFilesButton = new QPushButton("Dispatch: Show Files Sidebar");
    QPushButton* toBugsButton = new QPushButton("Dispatch: Show Bugs Sidebar");
    QPushButton* showProfileButton = new QPushButton("Dispatch: Show User Profile (Sidebar should ignore this)");

    controlsLayout->addWidget(toFilesButton);
    controlsLayout->addWidget(toBugsButton);
    controlsLayout->addSpacing(20);
    controlsLayout->addWidget(showProfileButton);
    controlsLayout->addStretch();

    QObject::connect(toFilesButton, &QPushButton::clicked, []() {
        Store::instance().dispatch({ActionType::SidebarChangeActivePage, std::string("filesSidebar")});
    });
    QObject::connect(toBugsButton, &QPushButton::clicked, []() {
        Store::instance().dispatch({ActionType::SidebarChangeActivePage, std::string("bugSidebar")});
    });
    // This action will NOT cause the sidebar to update its UI, because of Optimization 1
    QObject::connect(showProfileButton, &QPushButton::clicked, []() {
        Store::instance().dispatch({ActionType::UserProfileShow});
    });

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(controls);

    mainWindow.setCentralWidget(centralWidget);
    mainWindow.resize(600, 300);
    mainWindow.show();

    return app.exec();
}
#include "main.moc"
