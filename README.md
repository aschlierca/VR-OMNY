# VR-OMNY

Website: (https://sites.google.com/view/csc211h-seoyeoncho-sp26/home?authuser=0)

## Inspiration
New York City Metropolitan Transportation Authority released a new tap and pay system for public transportations using a new vending machine and OMNY card system. To provide a safe, low-pressure space for autistic individuals, this application helps user build transit confidence and independence before real-world application.

## Project Description
VR-OMNY is an interactive educational game built in Unreal Engine 5 (C++) that simulates the NYC Metropolitan Transportation Authority's OMNY contactless payment system for autistic users. Players navigate a realistic virtual subway environment, learning to purchase an OMNY card at a kiosk, add funds, and tap to enter subway gates through guided, step-by-step gameplay with clear visual and auditory feedback. The game provides a safe, low-pressure space for autistic individuals to build transit confidence and independence before real-world application.

## Algorithm

   Wallet ──pay──▶ Kiosk ──AddFunds──▶ OMNY Card ──Deduct──▶ Gate
                                            │
                                            └── OnBalanceChanged ──▶ HUD + Save

## Challenges

1. Accessibility: Understanding the context of how users would perceive the experience
2. C++ and Blueprint: Learning how to integrate C++, Blueprints, Input Mapping Context (IMC), Widgets (UI) that are unique to UE environment
3. Debugging & Version Control: UE5 is more supportive in Windows than MacOS; There were some crashes where I wasn’t able to commit before losing progress


## Accomplishment

1. UE environment: Integrating C++, Blueprint, IMC, Widget etc; C++ syntax and logic in UE5
2. Project: Implementing OMNY card vending machine using Widget system, C++, Blueprint


## Future Direction

1. Design with enhanced accessibility principles including high contrast, large text, audio narration, plain language, gamification
2. Add different scenarios and maps in different stations and bus navigation.
3. Make the environment more realistic and potentially revise into VR system for immersive experience
