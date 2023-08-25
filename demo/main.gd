extends Control


func _init() -> void:
	# Print time for debugging
	var date = Time.get_datetime_dict_from_system(false)
	date.period = "pm" if date.hour >= 12 and date.hour != 24 else "am"
	date.hour = (date.hour - 12 if date.hour > 12 else date.hour)
	date.minute = str(date.minute).pad_zeros(2)
	print("{month}/{day}/{year}, {hour}:{minute}{period}\n---\n".format(date))


func _ready() -> void:
	var file: RiveFile = $RiveViewer.get_file()
	prints("# Artboards:", file.get_artboard_count(), file.get_artboards())
	
	var artboard: RiveArtboard = $RiveViewer.get_artboard()
	prints("# Scenes: ", artboard.get_scene_count(), artboard.get_scenes())
	
	await get_tree().create_timer(1.0).timeout
	
	var scene := artboard.get_scene(0)
	scene.find_input("rating").set_value(2.0)
	
	var animation := artboard.get_animation(0)
	print(animation)
	
	
