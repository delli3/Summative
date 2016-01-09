#include "Physics.h"

Physics::Physics(Vector2D f){
	Gravforce = f;
}

void Physics::GravTick(std::vector<Character*> All){
	for (int i = 0; i < (int)All.size(); i++) {		//adding gravity to all characters registered
		if (!All[i]->bOnGround){
			All[i]->velocity.y += Gravforce.y;	//could theoretically implement an x gravity if ever wanted to
		}
		if (All[i]->velocity.y >= 50) {
			All[i]->velocity.y = 50;
		}
	}
}

int Physics::ColTick(World* Curr, std::vector<Character*> All){
	for (int j = 0; j < (int)All.size(); j++) {

		//Stop character from falling through a block
		if (!All[j]->bOnGround && Curr->Blocks[(int)((All[j]->GetCharacterWorldPosition().x + (All[j]->ActualWidth / 2)) / Curr->gridSize)][(int)((All[j]->GetCharacterWorldPosition().y + All[j]->ActualHeight) / Curr->gridSize)].bSpawned && Curr->Blocks[(int)((All[j]->GetCharacterWorldPosition().x + (All[j]->ActualWidth / 2)) / Curr->gridSize)][(int)((All[j]->GetCharacterWorldPosition().y + All[j]->ActualHeight) / Curr->gridSize)].bCollision) {
			TypeChecker = dynamic_cast<Barrel*>(All[j]);
			if (!TypeChecker)
				All[j]->SetCharacterWorldPosition(Vector2D(All[j]->GetCharacterWorldPosition().x, Curr->Blocks[(int)((All[j]->GetCharacterWorldPosition().x) / Curr->gridSize)][(int)(All[j]->GetCharacterWorldPosition().y) / Curr->gridSize].position.y));
			else 
				All[j]->SetCharacterWorldPosition(Vector2D(All[j]->GetCharacterWorldPosition().x, Curr->Blocks[(int)((All[j]->GetCharacterWorldPosition().x) / Curr->gridSize)][(int)(All[j]->GetCharacterWorldPosition().y) / Curr->gridSize].position.y + 16));
			TypeChecker = NULL;
			ColPos = Vector2D(0.f, 0.f);
			All[j]->bOnGround = true;
			if (All[j]->velocity.y > 0) {
				All[j]->velocity.y = 0;
			}
		}
		
		//makes characters fall if there is no collidable blocks under them
		else if (!Curr->Blocks[(int)((All[j]->GetCharacterWorldPosition().x + (All[j]->ActualWidth / 2)) / Curr->gridSize)][(int)((All[j]->GetCharacterWorldPosition().y + All[j]->ActualHeight) / Curr->gridSize)].bSpawned && Curr->Blocks[(int)((All[j]->GetCharacterWorldPosition().x + (All[j]->ActualWidth / 2)) / Curr->gridSize)][(int)((All[j]->GetCharacterWorldPosition().y + All[j]->ActualHeight) / Curr->gridSize)].bCollision) {
			All[j]->bOnGround = false;
		}


		if (Curr->Blocks[(int)((All[j]->GetCharacterWorldPosition().x) / 32)][int((All[j]->GetCharacterWorldPosition().y) / 32)].bSpawned && All[j]->velocity.y < 0 && Curr->Blocks[(int)((All[j]->GetCharacterWorldPosition().x) / 32)][int((All[j]->GetCharacterWorldPosition().y) / 32)].bCollision) {
			ColPos = Curr->Blocks[(int)((All[j]->GetCharacterWorldPosition().x) / 15)][int((All[j]->GetCharacterWorldPosition().y + 5) / 32)].position;
			All[j]->SetCharacterWorldPosition(Vector2D(All[j]->GetCharacterWorldPosition().x, ColPos.y));
			All[j]->velocity.y = 0;
		}
		if (j == 0) {
			for (int i = 33; i < 98; i += 32) {
				//This assumes that the first character is the player (can add a condition that casts it to check)
				if (!static_cast<bool>(All[j]->GetCharacterDirection()) && Curr->Blocks[(int)((All[j]->GetCharacterWorldPosition().x - 10) / 32)][int((All[j]->GetCharacterWorldPosition().y + i) / 32)].bSpawned && Curr->Blocks[(int)((All[j]->GetCharacterWorldPosition().x - 10) / 32)][int((All[j]->GetCharacterWorldPosition().y + i) / 32)].type == EBlockType::B_FinishFlag) {
					return 1;
				}
				if (static_cast<bool>(All[j]->GetCharacterDirection()) && Curr->Blocks[(int)((All[j]->GetCharacterWorldPosition().x + 74) / 32)][int((All[j]->GetCharacterWorldPosition().y + i) / 32)].bSpawned && Curr->Blocks[(int)((All[j]->GetCharacterWorldPosition().x + 74) / 32)][int((All[j]->GetCharacterWorldPosition().y + i) / 32)].type == EBlockType::B_FinishFlag) {
					return 1;
				}
				if (!static_cast<bool>(All[j]->GetCharacterDirection()) && Curr->Blocks[(int)((All[j]->GetCharacterWorldPosition().x + 65) / 32)][int((All[j]->GetCharacterWorldPosition().y + i) / 32)].bSpawned && Curr->Blocks[(int)((All[j]->GetCharacterWorldPosition().x + 65) / 32)][int((All[j]->GetCharacterWorldPosition().y + i) / 32)].bCollision) {		//all possible x related collisions
					All[j]->velocity.x = -5.f;
				}
				else if (static_cast<bool>(All[j]->GetCharacterDirection()) && Curr->Blocks[(int)((All[j]->GetCharacterWorldPosition().x + 1) / 32)][int((All[j]->GetCharacterWorldPosition().y + i) / 32)].bSpawned && Curr->Blocks[(int)((All[j]->GetCharacterWorldPosition().x + 1) / 32)][int((All[j]->GetCharacterWorldPosition().y + i) / 32)].bCollision) {
					All[j]->velocity.x = 5.f;
				}
			}
		}

	}
	return 0;
}

void Physics::FricTick(std::vector<Character*> All){
	for (int i = 0; i < (int)All.size(); i++) {
		TypeChecker = dynamic_cast<Barrel*>(All[i]);
		if (!TypeChecker) {
			if (All[i]->velocity.x == 0) {}
			else if (All[i]->velocity.x > 0 && (All[i]->bRunning || All[i]->bOnGround)) {
				All[i]->velocity.x -= 0.1f;
			}
			else if (All[i]->velocity.x < 0 && (All[i]->bRunning || All[i]->bOnGround)) {
				All[i]->velocity.x += 0.1f;
			}
		}
		TypeChecker = NULL;
	}
}