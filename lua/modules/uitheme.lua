
local uitheme = {
	_themes = {
		dark = {
			textColor = Color.White,
			textColorSecondary = Color(80,80,80),
			padding = 6,
			paddingBig = 12,
			gridSpacing = 4,
			gridCellColor = Color(0,0,0,0.6),
			gridCellColorPressed = Color(255,255,255,0.6),
			buttonColor = Color(63,63,63),
			buttonColorSecondary = Color(63,63,63,0.8),
			buttonTextColor = Color(240,240,240),
			buttonColorPressed = Color(40,40,40),
			buttonTextColorPressed = Color.White,
			buttonColorSelected = Color(70,2013,178),
			buttonTextColorSelected = Color.White,
			buttonColorDisabled = Color(20,20,20,0.5),
			buttonTextColorDisabled = Color(84,84,84,0.5),
			buttonTopBorderBrightnessDiff = 0.18,
			buttonBottomBorderBrightnessDiff = -0.15,
			buttonBorderSize = 3,
			textInputBorderSize = 2,

			modalBorder = 0,
			modalMargin = 12, -- minimum space around modal
			modalTabSpace = 3,
			modalTopBarColor = Color(63,63,63),
			modalBottomBarColor = Color(63,63,63),
			modalTabColorSelected = Color(63,63,63),
			modalTabColorPressed = Color(80,80,80),
			modalTabColorIdle = Color(28,28,28),

			colorNegative = Color(227,53,56),
			colorPositive = Color(139,195,74),
			colorExplore = Color(1,112,236),
			colorCreate = Color(236,103,11),
			colorDiscord = Color(90, 101, 234),

			textInputCursorWidth = 2,
			textInputCursorBlinkTime = 0.3,

			textInputDefaultWidth = 100,
			textInputBackgroundColor = Color(80,80,80),
			textInputBackgroundColorPressed = Color(150,150,150),
			textInputBackgroundColorFocused = Color(38,130,204),
			textInputBackgroundColorDisabled = Color(150,150,150),

			textInputTextColor = Color(230,230,230),
			textInputTextColorPressed = Color.White,
			textInputTextColorFocused = Color.White,
			textInputTextColorDisabled = Color(255,255,255,0.3),

			textInputPlaceholderColor = Color(0,0,0,0.2),
			textInputPlaceholderColorPressed = Color(0,0,0,0.2),
			textInputPlaceholderColorFocused = Color(0,0,0,0.2),
			textInputPlaceholderColorDisabled = Color(0,0,0,0.1),

			textInputBorderBrightnessDiff = -0.1,
		},
	},
	current = nil,
}

uitheme.current = uitheme._themes.dark

local meta = {
	__index = function(t,k)
		return t.current[k]
	end,
	__newindex = function(t,kv)
		error("can't set new keys in uitheme")
	end
}

setmetatable(uitheme, meta)

return uitheme