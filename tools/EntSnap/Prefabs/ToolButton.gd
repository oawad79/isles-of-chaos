extends ToolButton

func _ready():
	var f = connect("pressed", self, "_button_pressed")
	print(f)

func new_entity():
	pass

func _button_pressed():
	var node = get_node("../../TabContainer")
	
	var entityPanel = 
	
