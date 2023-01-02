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
#include <Window.h>
#include <vector>
#include <InputParser.h>

namespace rose {

/**
 * @class Application
 * @brief Abstraction of a Rose2 application.
 */
    class Application {
    protected:
        static constexpr std::string_view UsbDeviceByPath{"/dev/input/by-path/"};
        static constexpr std::string_view KeyboardPathRegEx{".*-kbd"};

        bool mKeyboardFound{false};     ///< Set to true if a keyboard is attached at startup.
        bool mRunEventLoop{true};       ///< The event loop runs while this is true.

        std::vector<std::unique_ptr<Window>>    mWindows{};

        InputParser mInputParser;

        GraphicsModel mGraphicsModel;

        Rectangle mWidowSizePos{};

        std::string mWindowName{};

        void basicEventLoop();

        void applicationDraw();

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

        bool initializeGraphics();

        auto begin() { return mWindows.begin(); }

        auto end() { return mWindows.end(); }

        template<class S>
        requires StringLike<S>
        void createWindow(S title, const Size &size, const Point &point, unsigned flags) {
            mWindows.emplace_back(std::make_unique<Window>());
            mWindows.back()->initialize(title, size, point, flags);
        }

        void run() {
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

#endif //ROSE2_APPLICATION_H
