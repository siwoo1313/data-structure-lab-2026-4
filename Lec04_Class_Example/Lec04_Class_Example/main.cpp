#include "Rectangle.h"
#include "Pet.h"
#include "Music.h"
using namespace std;
int main() {
	//Rectangle 1 객체 생성
	Rectangle rect1(3.4, 2.5);
	cout << "Rectangle 1 : " << endl;
	//위 코드는 다음 코드와 같음 printf("Rectangle 1:\n");
	cout << "Area : " << rect1.getArea() << endl;
	cout << "Perimeter : " << rect1.getPerimeter() << endl;
	cout << "Is Square ?:" << boolalpha << rect1.isSquare() << endl;
	printf("=====================================\n");
	// Pet 클래스 테스트
	Pet myPet("Kong", 5, "Cat");
	cout << "My pet's name is " << myPet.getName() << endl;
	// Puppy 클래스 테스트
	Puppy myPuppy("Lucky", 10, "Dog", "Puddle");
	cout << "My puppy's name is " << myPuppy.getBreed() << endl;
	printf("=====================================\n");
	// Streaming Service 클래스 테스트
	MusicStreamingService myService("Spotify");
	//음악 service에 추가하기
	myService.addMusic("SWIM", "BTS", "ARIRANG", 2026);
	myService.addMusic("RUDE", "Hearts2Hears", "RUDE", 2026);
	myService.addMusic("BANGBANG", "IVE", "REVIVE", 2026);
	myService.addMusic("JUMP", "BlackPink", "JUMP", 2025);
	myService.addMusic("ShutDown", "BlackPink", "BornPink", 2022);
	//title로 search
	string music_title;
	cout << "Enter the Music Title: ";
	cin >> music_title;
	//위 코드는 다음 코드와 같음 scanf_s("%s", % music_title);
	Music* result = myService.serchByTitle(music_title);
	if (result != NULL) {
		cout << "Found: " << result->getTitle() << " by " << result->getArtist() << endl;
	}
	else {
		cout << "Not Found" << endl;

	}

}