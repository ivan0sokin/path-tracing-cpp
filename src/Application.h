#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <functional>
#include <string>

class Application {
public:
    Application(int windowWidth, int windowHeight, const char *windowTitle) noexcept :
        m_WindowWidth(windowWidth), m_WindowHeight(windowHeight), m_WindowTitle(windowTitle) {}

    inline void SetOnUpdate(std::function<void()> onUpdate) noexcept {
        m_OnUpdate = onUpdate;
    }

    inline void SetOnWindowResize(std::function<void()> onWindowResize) noexcept {
        m_OnWindowResize = onWindowResize;
    }

    int Run() noexcept;

    inline  int GetWindowWidth() const {
        return m_WindowWidth;
    }

    inline  int GetWindowHeight() const {
        return m_WindowHeight;
    }

private:
    void OnWindowResize(int width, int height) noexcept;

private:
    int m_WindowWidth, m_WindowHeight;
    std::string m_WindowTitle;

    std::function<void()> m_OnUpdate = [](){};
    std::function<void()> m_OnWindowResize = [](){};
};

#endif