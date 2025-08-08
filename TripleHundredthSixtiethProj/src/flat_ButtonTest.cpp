#include "flat_Button.h"
#include "flat_EntityImpl.h"
#include "catch.hpp"

TEST_CASE("ButtonTests", "[click]")
{
	bool clicked = false;
	flat2d::ui::Button button(10, 10, 30, 10, [&]() { clicked = true; });

	REQUIRE(!clicked);

	button.triggerClick();

	REQUIRE(clicked);
}
