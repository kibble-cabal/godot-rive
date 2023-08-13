@tool
extends EditorImportPlugin


func _get_importer_name():
	return "addon.rive.importer"


func _get_visible_name():
	return "Rive File"


func _get_recognized_extensions():
	return ["riv"]


func _get_save_extension():
	return "txt"


func _get_import_options(path: String, preset_index: int) -> Array[Dictionary]:
	return []


func _get_preset_count() -> int:
	return 0


func _get_option_visibility(path: String, option_name: StringName, options: Dictionary) -> bool:
	return true


func _get_resource_type() -> String:
	return "Resource"


func _get_priority() -> float:
	return 1.0


func _get_import_order() -> int:
	return 0


func _import(source_file, save_path, options, r_platform_variants, r_gen_files):
	if not FileAccess.file_exists(source_file): return ERR_FILE_NOT_FOUND
	var bytes = FileAccess.get_file_as_bytes(source_file)
	var path := "%s.%s" % [save_path, _get_save_extension()]
	var import_file := FileAccess.open(path, FileAccess.WRITE)
	if import_file == null: return FileAccess.get_open_error()
	import_file.store_buffer(bytes)
	import_file.close()
	return OK
