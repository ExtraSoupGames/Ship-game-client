#include "Collision.h"
int FindOrientation(Vector2 P1, Vector2 P2, Vector2 P3) {
	double val = (((P2.y - P1.y) * (P3.x - P2.x)) - ((P2.x - P1.x) * (P3.y - P2.y)));
	if (val == 0) return 0;
	return (val > 0) ? 1 : 2;
}

void CollisionBoundary::Draw(SDL_Renderer* renderer) {
	SDL_RenderDrawLine(renderer, point1.x, point1.y, point2.x, point2.y);
}
CollisionBoundary::CollisionBoundary(double x1, double y1, double x2, double y2, double ox, double oy) {
	point1 = *new Vector2{ x1, y1 };
	point2 = *new Vector2{ x2, y2 };
	collideOffset = *new Vector2{ox, oy};
}
bool CollisionBoundary::IsRelevant(Vector2 start, Vector2 desiredEnd) {
	//check if the line of travel intersects with the line of the boundary at all
	double orientation1 = FindOrientation(start, desiredEnd, point1);
	double orientation2 = FindOrientation(start, desiredEnd, point2);
	double orientation3 = FindOrientation(point1, point2, start);
	double orientation4 = FindOrientation(point1, point2, desiredEnd);
	return ((orientation1 != orientation2) && (orientation3 != orientation4));
}
Vector2 CollisionBoundary::InterruptMovement(Vector2 start, Vector2 desiredEnd) {
	double A1 = desiredEnd.y - start.y;
	double B1 = start.x - desiredEnd.x;
	double C1 = A1 * start.x + B1 * start.y;
	double A2 = point2.y - point1.y;
	double B2 = point1.x - point2.x;
	double C2 = A2 * point1.x + B2 * point1.y;
	//Cramer's Rule
	double outX = ((C1 * B2) - (C2 * B1)) / ((A1 * B2) - (A2 * B1));
	double outY = ((A1 * C2) - (A2 * C1)) / ((A1 * B2) - (A2 * B1));
	double finalX = outX + collideOffset.x;
	double finalY = outY + collideOffset.y;
	return *new Vector2{finalX, finalY};
}
CollisionManager::CollisionManager() {
	boundaries = *new vector<CollisionBoundary>();
}
void CollisionManager::AddBoundary(CollisionBoundary newBoundary) {
	boundaries.push_back(newBoundary);
}
Vector2 CollisionManager::ApplyCollision(Vector2 start, Vector2 desiredEnd) {
	Vector2 realEnd = desiredEnd;
	int interrupts = 0;
	for (CollisionBoundary cb : boundaries) {
		if (cb.IsRelevant(start, desiredEnd)) {
			realEnd = cb.InterruptMovement(start, desiredEnd);
			interrupts++;
		}
	}
	if (interrupts > 1) {
		cout << "multiple lines collided with, one collision ignored, problems most likely occuring" << endl;
		//fix this to pass player along line
		//recursive call with location and new direction with shortened distance
	}
	return realEnd;
}
void CollisionManager::DrawDebug(SDL_Renderer* renderer) {
	for (CollisionBoundary cb : boundaries) {
		cb.Draw(renderer);
	}
}
bool CollisionManager::IsEmpty() {
	return boundaries.size() < 1;
}