// Store.cpp
#include "Store.h"
#include <QDebug>

Store& Store::instance() {
    static Store storeInstance;
    return storeInstance;
}

Store::Store(QObject* parent) : QObject(parent) {}

void Store::dispatch(const Action& action) {
    // Modify the state using the reducer
    reduce(action);

    // Notify all listeners that the state has changed
    emit stateChanged();
}

const AppState& Store::getState() const {
    return m_state;
}

// This is our "reducer"
void Store::reduce(const Action& action) {
    switch (action.type) {
    case ActionType::SidebarChangeActivePage: {
        // std::get safely extracts the value from the variant
        if (auto pageName = std::get_if<std::string>(&action.payload)) {
            m_state.sidebar.activePage = *pageName;
            qDebug() << "State Changed: Sidebar active page is now" << QString::fromStdString(m_state.sidebar.activePage);
        }
        break;
    }
    case ActionType::UserProfileShow: {
        m_state.userProfile.visible = true;
        qDebug() << "State Changed: User profile is now visible";
        break;
    }
    case ActionType::UserProfileHide: {
        m_state.userProfile.visible = false;
        qDebug() << "State Changed: User profile is now hidden";
        break;
    }
    }
}
