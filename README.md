# NBS Status Effects Plugin
## Purpose
This plugin is an expansion onto GAS to create a more tangible version of the `GameplayEffect` (GEs). The idea here is that GEs can be 
used for a wide variety of purposes, many of which are meant to be kept hidden from the player, whereas Status Effects are a subset of 
Gameplay Effects intended to be seen and felt by the player.

## Requirements
1. Unreal Engine 5.5 (Works with launcher build.)
2. [UE5Coro >2.0.1](https://github.com/landelare/ue5coro/releases/tag/v2.0.1)

## Installation
1. Clone this repository into a folder within your project's `Plugins` folder, and compile.

## Current Features
* Allows for conveniently tying graphical and audio assets to a Status Effect.
* Includes three base classes for creating a UI that shows current Status Effects.
* Supports both Infinite and Temporary effects. Temporary effects can show on the UI with an added progress bar.
* Fully Blueprint compatible. (The only exceptions are the override delegates for Adding/Removing status effects.
* Actor Component which allows for granting effects on overlap begin and end, and removing effects on overlap end.
* Debug functions for finding and granting effects by name.

## Planned Features
* "Status Effect Pad" actor which spawns status effect models and grants effects on overlap.
* Sample project with sample effects, such as a simple `ACharacter` speed boost.
* Automated tests for each feature.
* Example User Interface using existing classes.

## Usage
### Creating A Status Effect
1. Create a new `UGameplayEffect` blueprint in the editor.
2. Add a "Status Effect Data Component" to it, and fill out any relevant asset fields.
3. Proceed to add any modifiers, abilities, etc. as you normally would.
