# Qt6-Redux-Store

A simple, header-only, Redux-like state management library for C++ and Qt 6. This project provides a centralized store and a unidirectional data flow architecture, making it easier to manage and reason about the state of complex Qt applications.

It uses modern C++ features like `std::variant` for type-safe action payloads and leverages Qt's core **Signals and Slots** mechanism to create reactive UIs that automatically update when the state changes.

## Core Concepts

This library maps common Redux concepts directly to C++/Qt idioms.

| Redux Concept                  | C++ / Qt Equivalent                                      |
| :----------------------------- | :------------------------------------------------------- |
| **State** (JS Object)          | A `struct` (e.g., `AppState`)                            |
| **Action** (JS Object)         | A `struct` with an `enum class` and a `std::variant` payload |
| **Store** | A Singleton `QObject` class (`Store`)                    |
| **Dispatch Function** | A public method: `Store::instance().dispatch(...)`         |
| **Reducer Function** | A private method within the `Store`                      |
| **Reacting to Changes** (`useSelector`) | Connecting a UI slot to the `Store::stateChanged` signal   |

## Features

✅ **Centralized State:** A single `AppState` struct serves as the single source of truth.
✅ **Unidirectional Data Flow:** State is read-only and can only be changed by dispatching actions.
✅ **Type-Safe Payloads:** Uses `std::variant` to ensure action payloads are type-safe at compile time.
✅ **Reactive UI:** Components connect to a global `stateChanged` signal and update themselves automatically.
✅ **Singleton Access:** The store is globally accessible via `Store::instance()`, simplifying integration.
✅ **Header-Only:** The entire store is contained in two headers (`State.h`, `Store.h`) for easy inclusion in any project.

## How to Use

#### 1. Define Your State and Actions (`State.h`)
Create a header to define the shape of your application's state and all possible actions.

```cpp
// State.h
#pragma once
#include <string>
#include <variant>

struct SidebarState { /* ... */ };
struct AppState {
    SidebarState sidebar;
    // ... other state slices
};

enum class ActionType { /* ... */ };
using ActionPayload = std::variant<std::monostate, std::string, bool, int>;
struct Action {
    ActionType type;
    ActionPayload payload = std::monostate{};
};
```

#### 2. Include the Store (`Store.h`)
Add the `Store.h` header to your project. This file contains the complete implementation.

#### 3. Connect UI Components to the Store
In your widget's constructor, connect a slot to the store's `stateChanged` signal. This slot is where you will update your UI.

```cpp
// MyWidget.cpp
#include "Store.h"

MyWidget::MyWidget(QWidget* parent) : QWidget(parent) {
    // ... setup your UI ...

    // Connect to the store's signal
    connect(&Store::instance(), &Store::stateChanged, this, &MyWidget::onStateChanged);
    
    // Set the initial state
    onStateChanged();
}

void MyWidget::onStateChanged() {
    // Get the latest state
    const auto& state = Store::instance().getState();

    // Check if the data this widget cares about has changed
    if (m_lastKnownValue != state.someValue) {
        m_lastKnownValue = state.someValue;
        // Update the UI
        m_label->setText(m_lastKnownValue);
    }
}
```

#### 4. Dispatch Actions
From anywhere in your application (e.g., a button click), dispatch actions to modify the state.

```cpp
// MainWindow.cpp
#include "Store.h"

// ... inside a button's clicked connection
connect(myButton, &QPushButton::clicked, [](){
    Store::instance().dispatch({ActionType::SomeAction, std::string("New Value")});
});
```
The store will update its state, emit the `stateChanged` signal, and any connected widgets will automatically react.

## Complete Example

Here are the files for a complete, working example that demonstrates how to swap between complex widgets in a sidebar.

<details>
<summary>Click to expand code</summary>

**`State.h`**
```cpp
#pragma once
#include <string>
#include <variant>

struct SidebarState { std::string activePage = "chatList"; };
struct UserProfileState { bool visible = false; };
struct AppState { SidebarState sidebar; UserProfileState userProfile; };

enum class ActionType { SidebarChangeActivePage, UserProfileShow, UserProfileHide };
using ActionPayload = std::variant<std::monostate, std::string, bool, int>;
struct Action { ActionType type; ActionPayload payload = std::monostate{}; };
```

**`Store.h`** (Header-only implementation)
```cpp
#pragma once
#include <QObject>
#include <QDebug>
#include "State.h"

class Store : public QObject {
    Q_OBJECT
public:
    static Store& instance() {
        static Store storeInstance;
        return storeInstance;
    }
    void dispatch(const Action& action) {
        reduce(action);
        emit stateChanged();
    }
    const AppState& getState() const {
        return m_state;
    }
signals:
    void stateChanged();
private:
    Store(QObject* parent = nullptr) : QObject(parent) {}
    AppState m_state;
    void reduce(const Action& action) {
        switch (action.type) {
            case ActionType::SidebarChangeActivePage: {
                if (auto pageName = std::get_if<std::string>(&action.payload)) {
                    m_state.sidebar.activePage = *pageName;
                }
                break;
            }
            case ActionType::UserProfileShow:
                m_state.userProfile.visible = true;
                break;
            case ActionType::UserProfileHide:
                m_state.userProfile.visible = false;
                break;
        }
    }
    Store(const Store&) = delete;
    Store& operator=(const Store&) = delete;
};
```

**`main.cpp`**
```cpp
#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <map>
#include "Store.h"

// ... (Sidebar and main() implementation from our previous conversation) ...
```
</details>

## Dependencies
* **Qt 6** (`QtCore` module)
* **C++17** (for `std::variant`)

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
