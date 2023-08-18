extends Control


func _ready() -> void:
	# Print time for debugging
	var date = Time.get_datetime_dict_from_system(false)
	date.period = "pm" if date.hour >= 12 and date.hour != 24 else "am"
	date.hour = (date.hour - 12 if date.hour > 12 else date.hour)
	date.minute = str(date.minute).pad_zeros(2)
	print("{month}/{day}/{year}, {hour}:{minute}{period}".format(date))
	
	$RiveViewer.test()
