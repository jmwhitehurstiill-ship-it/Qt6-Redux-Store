// Store.h
#pragma once

#include <QObject>
#include "State.h"

class Store : public QObject {
    Q_OBJECT

public:
    // This makes it a Singleton - you access it via Store::instance()
    static Store& instance();

    // Public method to dispatch an action
    void dispatch(const Action& action);

    // Public method to get the current state
    const AppState& getState() const;

signals:
    // This signal is emitted whenever the state changes
    void stateChanged();

private:
    // Private constructor to enforce the Singleton pattern
    Store(QObject* parent = nullptr);

    // The single source of truth for the application state
    AppState m_state;

    // The "reducer" function that modifies the state based on an action
    void reduce(const Action& action);

    // Make sure it can't be copied or assigned
    Store(const Store&) = delete;
    Store& operator=(const Store&) = delete;
};
