#ifndef CAR_GADGET_SELECTOR_H
#define CAR_GATGET_SELECTOR_H

#include "Element.h"
#include "../../Vehicle.h"
#include "ItemSlot.h"

class CarGadgetSelector : public Element
{
private:
	Texture* textureBG;
	Texture* textureCar;
	Texture* textureIndicator;

	Element* selected;
	std::unique_ptr<ItemSlot> slots[Slots::SIZEOF];
	Container::Slot* used[Slots::SIZEOF];
public:
	static Vector2 size;

	CarGadgetSelector(Vector2 position = Vector2());
	virtual ~CarGadgetSelector();

	void draw(bool selected);
	void update(bool selected, float deltaTime);
	void init();

	void setSlot(Slots slots, Container::Slot* slot);
	void setSlotOfSelected(Container::Slot* slot);
	void removeSlot(Container::Slot* slot);
	Container::Slot** getUsed();
};

#endif // !CAR_GADGET_SELECTOR_H
