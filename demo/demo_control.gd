extends VBoxContainer


func _on_h_slider_value_changed(value: float) -> void:
	(%RiveViewer as RiveViewer).get_scene().find_input("Rooms").set_value(value)
