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
        bool handleMouseMotionEvent(const SDL_MouseMotionEvent &e) {
            if ((e.state & (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK | SDL_BUTTON_MMASK)) == 0) {
                if (auto gadget = mouseMotionEvent(e); gadget) {
                    if (!mMouseGadget.expired()) {
                        if (auto oldGadget = mMouseGadget.lock(); oldGadget != gadget) {
                            fmt::print("Leave event: {}\n", oldGadget->mName);
                            fmt::print("Enter event: {}\n", gadget->mName);
                            mMouseGadget = gadget;
                        }
                    } else {
                        fmt::print("Enter event: {}\n", gadget->mName);
                        mMouseGadget = gadget;
                    }
                    return true;
                }
            }
            return false;
        }

        std::shared_ptr<Gadget> mouseMotionEvent(const SDL_MouseMotionEvent &e);

        void winStateChangeEvent(WindowEventType type, const SDL_WindowEvent &e);

    public:
        Application() = delete;

        Application(int argc, char **argv);

        std::shared_ptr<Application> createApplication(int argc, char **argv);

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
    };

#if 0
    /**
     * @class Screen
     * @brief An abstraction of the available display screen.
     */
    class Screen /*: public Visual, public Container*/ {
        Application &mApplication;

    public:
        explicit Screen(Application &application);

        ~Screen() override = default;

        static constexpr std::string_view id = "Screen";

        std::string_view nodeId() const noexcept override {
            return id;
        }

        /**
         * @brief Add a Window to the screen.
         * @param node The Window to add.
         */
        void add(const std::shared_ptr<Node> &node) override {
            if (auto window = std::dynamic_pointer_cast<Window>(node); window)
                Container::add(node);
            else
                throw NodeTypeError("A Screen may only contain Window objects.");
        }

        /// Draw the screen contents.
        void draw(gm::Context &context, const Position<int> &containerPosition) override;

        /// Layout the screen contents.
        Rectangle layout(gm::Context &context, const Rectangle &screenRect) override;

        /// Get the Application object associated with the Screen.
        Application &getApplication() {
            return mApplication;
        }

        /// Get the Application associated with a const Screen.
        const Application &getApplication() const {
            return mApplication;
        }
    };
#endif

} // rose

void operator >> (rose::Builder builder, std::shared_ptr<rose::Application> &application) {
    application->window().value()->gadget<rose::Widget>().value()->manage(builder.get<rose::Gadget>());
}


#endif //ROSE2_APPLICATION_H
