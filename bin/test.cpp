#include <ev/core/execution_context.hpp>
#include <ev/core/executor.hpp>

#include <evt/gui/ExecutionContextModel.hpp>

#include <QTreeView>
#include <QApplication>

#define MAX 5

ev::executor executor{4};

auto sub_producer = [&](ev::execution_context context) {

    context.set_status(ev::execution_status::running);
    for (int i = 0; i < MAX; ++i) {
        context.set_progress(100 * i / MAX);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    context.set_progress(100);
    context.set_status(ev::execution_status::finished);
};

auto producer = [&](ev::execution_context context) {

    context.set_status(ev::execution_status::running);
    for (int i = 0; i < MAX; ++i) {
        ev::info(context) << "producer message" << i;
        context.set_progress(100 * i / MAX);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    ev::info(context) << "Starting new tasks";
    auto c1 = context.create_sub_ctx();
    c1.set_name("sub c1");
    auto c2 = context.create_sub_ctx();
    c2.set_name("sub c2");

    auto f1 = executor.async(sub_producer, c1);
    auto f2 = executor.async(sub_producer, c2);

    ev::debug(context) << "waiting for f1";
    f1.wait();

    ev::debug(context) << "waiting for f2";
    f2.wait();

    ev::info(context) << "Finished";

    context.set_progress(100);
    context.set_status(ev::execution_status::finished);
};

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
        QMetaObject::invokeMethod(&model, &evt::ExecutionContextModel::reset);
        QMetaObject::invokeMethod(&view, &QTreeView::expandAll);
    });

    auto c1 = context.create_sub_ctx();
    c1.set_name("C1");

    auto c2 = context.create_sub_ctx();
    c2.set_name("C2");

    auto p1 = executor.async(producer, c1);
    auto p2 = executor.async(producer, c2);

    return app.exec();
}
