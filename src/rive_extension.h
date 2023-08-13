#ifndef RIVEEXTENSION_H
#define RIVEEXTENSION_H

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

    class RiveNode : public Control {
        GDCLASS(RiveNode, Control);

       private:
        String filename;

       protected:
        static void _bind_methods() {
            ClassDB::bind_method(D_METHOD("test"), &RiveNode::test);
            ClassDB::bind_method(
                D_METHOD("get_filename"),
                &RiveNode::get_filename
            );
            ClassDB::bind_method(
                D_METHOD("set_filename", "value"),
                &RiveNode::set_filename
            );
            ClassDB::add_property(
                get_class_static(),
                PropertyInfo(Variant::STRING, "filename"),
                "set_filename",
                "get_filename"
            );
        }

       public:
        RiveNode();
        ~RiveNode();

        void test();

        String get_filename() {
            return filename;
        }

        void set_filename(String value) {
            filename = value;
        }
    };

}  // namespace godot

#endif