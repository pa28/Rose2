//
// Created by richard on 01/01/23.
//

/*
 * Application.h Created by Richard Buckley (C) 01/01/23
 */

#ifndef ROSE2_APPLICATION_H
#define ROSE2_APPLICATION_H

#include <Rose.h>
#include <InputParser.h>
#include <GraphicsModel.h>
#include <vector>
#include <memory>
#include <InputParser.h>
#include <Window.h>
#include <Event.h>
#include <Theme.h>

namespace rose {

    /**
     * @class Application
     * @brief Abstraction of a Rose2 application.
     */
    class Application : public std::enable_shared_from_this<Application> {
    protected:
        static constexpr std::string_view UsbDeviceByPath{"/dev/input/by-path/"};
        static constexpr std::string_view KeyboardPathRegEx{".*-kbd"};

        bool mKeyboardFound{false};     ///< Set to true if a keyboard is attached at startup.
        bool mRunEventLoop{true};       ///< The event loop runs while this is true.

        std::vector<std::shared_ptr<Window>>    mWindows{};

        std::weak_ptr<Window> mMouseWindow{};

        std::weak_ptr<Gadget> mMouseGadget{};

        InputParser mInputParser;

        GraphicsModel mGraphicsModel;

        Rectangle mWidowSizePos{};

        Event event{};

        std::string mWindowName{};

        void basicEventLoop();

        void applicationDraw();

        /**
         * @brief Evaluate mouse motion events.
         * @details The action performed depends on the buttons pressed.
         * If no buttons are pressed the Gadget that contains the mouse pointer is sent to the MouseGadgetStatus
         * object for processing.
         * @param e the mouse motion event.
         * @return true if handled, false if not.
         */
        bool handleMouseMotionEvent(const SDL_MouseMotionEvent &e);

        /**
         * @brief Evaluate mouse button events.
         * @param e the mouse button event.
         * @return true if handled, false if not.
         */
        bool handleMouseButtonEvent(const SDL_MouseButtonEvent &e);

        /**
         * @brief Find Gadget associated with a mouse motion event.
         * @param point the mouse motion event.
         * @return std::shared_ptr<Gadget> pointing to the Gadget, or empty if none found.
         */
        std::shared_ptr<Gadget> mousePointerToGadget(const Point &point);

        /**
         * @brief Ensure the mouse gadget is up to date with mouse location.
         * @param point The current location of the mouse.
         * @param timestamp The timestamp of the location.
         * @return The current mouse gadget.
         */
        std::shared_ptr<Gadget> validateMouseGadget(const Point &point, Uint32 timestamp);

        void winStateChangeEvent(WindowEventType type, const SDL_WindowEvent &e);

        void winSizeChange(WindowEventType type, const SDL_WindowEvent &e);

        Theme theme{};

    public:

        Application() = delete;

        Application(int argc, char **argv);

        static std::shared_ptr<Application> createApplication(int argc, char **argv);

        Application& operator=(const Rectangle &rectangle) {
            mWidowSizePos = rectangle;
            return *this;
        }

        Application& operator=(const std::string &windowName) {
            mWindowName = windowName;
            return *this;
        }

        std::optional<std::shared_ptr<Window>> window(size_t idx = 0) {
            if (idx < mWindows.size()) {
                return mWindows.at(idx);
            } else {
                return std::nullopt;
            }
        }

        bool initializeGraphics();

        auto begin() { return mWindows.begin(); }

        auto end() { return mWindows.end(); }

        template<class S>
        requires StringLike<S>

        void createWindow(S title, const Size &size, const Point &point, unsigned flags) {
            auto window = Window::createWindow();
            window->initialize(shared_from_this(), title, size, point, flags);
            mWindows.push_back(std::move(window));
        }

        void run() {
            mWindows.front()->layout();
            basicEventLoop();
        }

        [[nodiscard]] std::string applicationName() const;

        Theme& getTheme() { return theme; }
    };

} // rose

inline void operator >> (rose::Builder builder, std::shared_ptr<rose::Application> &application) {
    application->window().value()->gadget<rose::Widget>().value()->manage(builder.get<rose::Gadget>());
}


#endif //ROSE2_APPLICATION_H
