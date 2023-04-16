## Reforger Shop System

## Description
This mod adds an expandable shop system to Arma Reforger, and eventually Arma 4. Current features include buying items using a currency, or bartering with other items. The system is designed to be expandable which allows new types of merchandise to be added if the system doesn't currently support it, as well as new types of payment methods. 

Currently the system allows two types of payment methods: item, and currency. As well as two types of shops: physical shop, and a traditional UI based shop. Physical shops display the merchandise being sold as they normally apear in the world, and UI shops display all items in list.

## Installation
You can use this mod as a dependency from the released version on the workshop, or you can incorporate the code into your own codebase.

## Usage
There are two main components supported right now. ADM_ShopComponent and ADM_PhysicalShopComponent. These should be added to an entity in the world which you want to represent the shop. This can be an AI character, or an inanimate object. In either case an ActionsManager component is required. For normal UI-based shops, add ADM_ShopAction to the "Additional Actions" section. For physical shops, add ADM_PhysicalShopAction to the "Additional Actions".

### ADM_ShopComponent
![UI Based Shop](https://i.imgur.com/qAC1fED.png)

To begin configuring, select the component.

![](https://i.imgur.com/lz0exBV.png)

In this component you have the option to add a preconfigured shop config (this can be for having multiple traders of the same type around a map).

If you don't have a config you can add items directly to the "Additional Merchandise" array.

When you add a merchandise item, it will ask you to select a class.

![](https://i.imgur.com/WmjHUpz.png)

There are two options (ADM_MerchandiseType is a base class and does not do anything): ADM_MerchandiseItem, and ADM_MerchandiseVehicle. ADM_MerchandiseItem is for any inventory item or weapon, ADM_MerchandiseVehicle is for any type of vehicle. 

![](https://i.imgur.com/PFEWptM.png)

ADM_MerchandiseItem requires you to specify the prefab from the Resource Browser. There is also an option to allow sale with a full inventory which would instead drop the item on the ground upon purchase.

![](https://i.imgur.com/sN69lTs.png)

ADM_MerchandiseVehicle also requires you to specify a prefab from the Resource Browser. The spawn position PointInfo is optional. When set it is relative to the shop. If it is unspecified then it will spawn directly where the shop is, for UI-based shops vehicles will be unable to spawn if there is any overlapping collisions with an unspecified spawn position.

Next you will need to define the required payments. Currently there are two supported payment methods:

![](https://i.imgur.com/4MboLFb.png)

ADM_PaymentMethodCurrency and ADM_PaymentMethodItem. Both allow you to specify a quantity. ADM_PaymentMethodItem allows you to specify the prefab of the item desired for payment.

![](https://i.imgur.com/XF5383G.png)

![](https://i.imgur.com/Fidfbns.png)

The mod comes with a basic currency implementation which can easily be overridden for any custom implementations. See ADM_PaymentMethodCurrency class for more information. The default currency system stores a quantity of currency as a variable in a component on an item. This item can be anything and is chosen to be a wallet for simplicity. Using individual entities as currency will work, but when dealing with large amounts such as $100,000 or $1,000,000 there would be an excessive amount of entities which would all be in the world, eventually becoming unfeasable. This is the reason for storing the currency quantity as a variable.

Finally you can specify a list of categories for the shop. These are configs with a list of prefabs that are within each category, a few default ones have been provided for reference.

### ADM_PhysicalShopComponent
The setup of a physical shop is similar to a normal shop. You can define the merchandise to be sold by setting the class on the "Physical Merchandise" field, and then selecting the merchandise type. See above for options. If the physical merchandise field is set then the config is ignored. A respawn time can be set, it is measured in seconds. If set to -1 then no respawn will occur and only one item can be purchased from the shop.

### Closing Remarks
If you have your own player controller prefab you must add the ADM_PlayerShopManager component to it. This controls communicating to the server to request purchasing. The project overrides the DefaultPlayerControllerMP prefab to achieve this.

## Support
Reach out on the Arma Discord server under the thread on the #enf_showcases channel.

## Roadmap
Planned Features:
 - Selling Items
   - Need to serialize the merchandise array
 - Limited Quantity
 - Dynamic prices based on supply & demand
 - Hooks for gamemodes to perform additional logic on transactions (ex: update database)
 - Weapon shop similar to GTA 5
   - Weapon attachement configuration similar to Call of Duty

## Known Bugs
 - Have physical shops show all default prefab slots
 - Physical shops will respawn inside of a recently purchased item if the respawn delay is set too low.

## Contributing
Feel free to make modifications and fix bugs. Submit a push request and I'll take a look at what you've done.

## License
This work is released under the Arma Public License Share Alike (APL-SA). Details are included in the LICENSE file as well as at this link: https://www.bohemia.net/community/licenses/arma-public-license.