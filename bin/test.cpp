#include <ev/core/execution_context.hpp>
#include <ev/core/executor.hpp>

#include <evt/gui/ExecutionContextModel.hpp>

#include <QTreeView>
#include <QApplication>

#define MAX 100
int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    ev::execution_context context;

    evt::ExecutionContextModel model;
    model.setExecutionContext(context);

    QTreeView view;
    view.setModel(&model);
    view.show();

    context.set_observer([&](auto&&, auto&&) {
        ev::debug() << __PRETTY_FUNCTION__;
        model.setExecutionContext(context);
    });

    auto producer = [](ev::execution_context context) {
        for (int i = 0; i < MAX; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            ev::debug(context) << i;
        }
    };

    ev::executor executor;

    auto c1 = context.create_sub_ctx();
    c1.set_name("C1");

    auto c2 = context.create_sub_ctx();
    c2.set_name("C2");

    auto p1 = executor.async(producer, c1);
    auto p2 = executor.async(producer, c2);

    return app.exec();
}
