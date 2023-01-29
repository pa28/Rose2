/**
* @file Application.h
* @author Richard Buckley <richard.buckley@ieee.org>
* @version 1.0
* @date 2023-01-01
* @brief The main application object.
*/

#ifndef ROSE2_APPLICATION_H
#define ROSE2_APPLICATION_H

#include <Rose.h>
#include <InputParser.h>
#include <GraphicsModel.h>
#include <vector>
#include <memory>
#include <InputParser.h>
#include "manager/Window.h"
#include <Event.h>
#include <Theme.h>
#include <Signals.h>
#include <TimerTick.h>

namespace rose {

    /**
     * @class Application
     * @brief Abstraction of a Rose2 application.
     */
    class Application : public std::enable_shared_from_this<Application> {
    protected:
        bool mNeedsLayout{true};            ///< True if application or a contained Gadget needs layout.
        bool mNeedsDrawing{true};           ///< True if application or a contained Gadget needs drawing.

        static constexpr std::string_view UsbDeviceByPath{"/dev/input/by-path/"};
        static constexpr std::string_view KeyboardPathRegEx{".*-kbd"};

        bool mKeyboardFound{false};     ///< Set to true if a keyboard is attached at startup.
        bool mRunEventLoop{true};       ///< The event loop runs while this is true.

        TimerTick mTimer{};             ///< Application real time signal source.

        std::vector<std::shared_ptr<Window>>    mWindows{};     ///< The list of attached Windows.

        std::weak_ptr<Window> mMouseWindow{};   ///< The Window that currently has the mouse, if any.

        std::weak_ptr<Gadget> mMouseGadget{};   ///< The Gadget that currently has the mouse, if any.

        InputParser mInputParser;   ///< InputParser for the command line arguments.

        GraphicsModel mGraphicsModel;   ///< The GraphicsModel abstraction of the SDL library.

        Rectangle mWidowSizePos{};      ///< The window size and position.

        Event event{};                  ///< The current event.

        std::string mWindowName{};      ///< The application window name.

        /**
         * @brief Process SDL events.
         */
        void basicEventLoop();

        /**
         * @brief Draw the application scene.
         */
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

        /**
         * @brief Handle window state change events.
         * @param type the SDL Window event type.
         * @param e the SDL Window event structure.
         */
        void winStateChangeEvent(WindowEventType type, const SDL_WindowEvent &e);

        /**
         * @brief Handle window size change events.
         * @param type The SDL Window event type.
         * @param e the SDL Window event structure.
         */
        void winSizeChange(WindowEventType type, const SDL_WindowEvent &e);

        std::shared_ptr<Theme> mTheme{};    ///< Storage for the application Theme.

    public:

        Application() = delete;

        Application(int argc, char **argv);

        Application& operator=(const Rectangle &rectangle) {
            mWidowSizePos = rectangle;
            return *this;
        }

        Application& operator=(const std::string &windowName) {
            mWindowName = windowName;
            return *this;
        }

        /**
         * @brief Get a pointer to one of the windows.
         * @param idx The index selects the window.
         * @return std::shared_ptr<Window> which will be empty if there is no Window at idx.
         */
        std::shared_ptr<Window> window(size_t idx = 0) {
            if (idx < mWindows.size()) {
                return mWindows.at(idx);
            } else {
                return nullptr;
            }
        }

        /**
         * @brief The animation signal.
         * @details The animation signal is used to provide execution time to subscribing Gadgets once
         * each time through the event loop.
         */
        AnimationProtocol::signal_type animationSignal{};

        /**
         * @brief Accessor for the system real time signal source.
         * @return A reference to a TimerTick.
         */
        [[maybe_unused]] TimerTick& timer() { return mTimer; }

        /**
         * @return The current value of the needs layout flag.
         */
        [[maybe_unused]] bool needsLayout() const { return mNeedsLayout; }

        /**
         * @return The current value of the needs drawing flag.
         */
        [[maybe_unused]] bool needsDrawing() const { return mNeedsDrawing; }

        /**
         * @brief Sets the needs layout flag to true.
         */
        void setNeedsLayout() {
            mNeedsLayout = true;
        }

        /**
         * @brief Sets the needs drawing flag to true.
         */
        void setNeedsDrawing() {
            mNeedsDrawing = true;
        }

        /**
         * Initialize the SDL graphics library.
         * @return true on success.
         */
        bool initializeGraphics();

        /**
         * @brief Return an iterator pointing to the first Window held by the Application.
         * @return A random access iterator.
         */
        auto begin() { return mWindows.begin(); }

        /**
         * @brief Return an iterator pointing to the past-the-end Window held by the Application.
         * @return A random access iterator.
         */
        auto end() { return mWindows.end(); }

        /**
         * @brief Create a new SDL Window attached to the Application.
         * @tparam S A string-like type
         * @param title The window title.
         * @param size The window size.
         * @param point The window position.
         * @param flags SDL Window creation flags.
         */
        template<class S>
        requires StringLike<S>
        void createWindow(S title, const Size &size, const Point &point, unsigned flags) {
            auto window = Window::createWindow();
            window->initialize(shared_from_this(), title, size, point, flags);
            mWindows.push_back(std::move(window));
        }

        /**
         * @brief Run the application.
         * @details Initialize the scene tree then start the event loop.
         */
        void run() {
            for (auto & window : mWindows) {
                window->layout();
                window->initializeSceneTree();
            }

//            SDL_Cursor *cursor;
//            cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
//            SDL_SetCursor(cursor);
            basicEventLoop();
        }

        /**
         * @brief Access the application name.
         * @return std::string.
         */
        [[nodiscard]] std::string applicationName() const;

        /**
         * @brief Access the application Theme.
         * @return std::shared_ptr<Theme>.
         */
        std::shared_ptr<Theme> getTheme() { return mTheme; }

        /**
         * @brief Get a shared pointer to the application.
         * @return std::shared_ptr<Application>.
         */
        std::weak_ptr<Application> getApplicationPtr() {
            return shared_from_this();
        }

        template<class GadgetType>
        requires std::derived_from<GadgetType,Gadget>
        void manage(std::shared_ptr<GadgetType>& gadget) {
            window()->gadget<rose::Widget>().value()->manage(gadget);
        }
    };

} // rose

#endif //ROSE2_APPLICATION_H
