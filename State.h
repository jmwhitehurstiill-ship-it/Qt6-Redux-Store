// State.h
#pragma once

#include <string>
#include <variant>

// The data for just the sidebar
struct SidebarState {
    std::string activePage = "chatList";
    std::string headerComponent = "ChatHeader";
};

// The data for just the user profile
struct UserProfileState {
    bool visible = false;
    std::string activePage = "UserProfile";
};

// The complete application state
struct AppState {
    SidebarState sidebar;
    UserProfileState userProfile;
};

// All possible actions that can change the state
enum class ActionType {
    SidebarChangeActivePage,
    UserProfileShow,
    UserProfileHide
};

// An action consists of a type and an optional payload
// std::variant is a modern C++ way to hold one of several possible types
using ActionPayload = std::variant<std::monostate, std::string, bool, int>;

struct Action {
    ActionType type;
    ActionPayload payload = std::monostate{}; // Default to no payload
};
