#include <iostream>
#include <vector>

#include "data_handler.h"


class ExplosionData : public DataClass {
public:
	ExplosionData(int pos) : _pos(pos), _damage(60) {};

	int getPosition()	const	{return _pos;}
	int getDamage()		const	{return _damage;}

private:
	int _pos, _damage;
};

class Enemy
{
public:
	Enemy() : _name("Bad Enemy"), _damage(15) {};
	
	std::string getName()	const {return _name;}
	int			getDamage()	const {return _damage;}

private:
	std::string _name;
	int _damage;
};

class EnemyHitData : public DataClass
{
public:
	EnemyHitData(Enemy* enemy) : _enemy(enemy){}

	Enemy* getEnemy() const		{return _enemy;}

private:
	Enemy* _enemy;
};


class Monster : public DataHandler
{
public:
	Monster() : _pos(20), _hp(100)
	{
		registerDataHandlerFunc(this, &Monster::onExplosion);
		registerDataHandlerFunc(this, &Monster::onEnemyHit);
	}

	void receiveDamage(int dmg)
	{
		_hp -= dmg;
		std::cout << "hp: " << _hp << std::endl;
		if(_hp <= 0)
		{
			std::cout << "I'm dead" << std::endl;
		}
	}
#if USE_PTR
	void onExplosion(const ExplosionData* explosion) {
		if(abs(_pos - explosion->getPosition()) < 20) {
			std::cout << "Hit by explosion!!!" << std::endl;
			receiveDamage(explosion->getDamage());
		}
		else {
			std::cout << "Out of explosion range :)" << std::endl;
		}
	}
		
	void onEnemyHit(const EnemyHitData* hit) {
		Enemy* enemy = hit->getEnemy();
		std::cout << "Hit by enemy: " << enemy->getName().c_str() << " got damage: " << enemy->getDamage() << std::endl;
		receiveDamage(enemy->getDamage());
	}
#else
	void onExplosion(const ExplosionData& explosion) {
		if (abs(_pos - explosion.getPosition()) < 20) {
			std::cout << "Hit by explosion!!!" << std::endl;
			receiveDamage(explosion.getDamage());
		}
		else {
			std::cout << "Out of explosion range :)" << std::endl;
		}
	}
	
	void onEnemyHit(const EnemyHitData& hit) {
		Enemy* enemy = hit.getEnemy();
		std::cout << "Hit by enemy: " << enemy->getName().c_str() << " got damage: " << enemy->getDamage() << std::endl;
		receiveDamage(enemy->getDamage());
	}
#endif

	int _pos;
	int _hp;
};

class Tank
{
public:
	Tank() : _dataHandler(this) {}

#if USE_PTR
	void handle(const DataClass* _data) {
		if (_data)
#else
		void handle(const DataClass& _data) {
#endif
		{
			_dataHandler.handle(_data);
		}
	}

private:
	class TankDataHandler : public DataHandler
	{
	public:
		TankDataHandler(Tank* tank) : _tank(tank) {
			registerDataHandlerFunc(_tank, &Tank::onDamagedByExplosion);
		}
		Tank* _tank;
	};
	//friend class TankEventHandler; // not needed with C++11
#if USE_PTR
	void onDamagedByExplosion(const ExplosionData* explosion) {
		std::cout << "Hit by explosion. Whatever" << std::endl;
	}
#else
	void onDamagedByExplosion(const ExplosionData& explosion) {
		std::cout << "Hit by explosion. Whatever" << std::endl;
	}
#endif
	TankDataHandler _dataHandler;
};


int main()
{
	std::cout << "=== Monster ===" << std::endl; 
	Monster monster;
	Tank tank;

#if USE_PTR
	monster.handle(new ExplosionData(40));
	monster.handle(new ExplosionData(10));
	monster.handle(new EnemyHitData(new Enemy));
	monster.handle(new EnemyHitData(new Enemy));
	monster.handle(new EnemyHitData(new Enemy));

	std::cout << "\n=== Tank ===" << std::endl;
	tank.handle(new ExplosionData(40));
	tank.handle(new ExplosionData(10));
	tank.handle(new EnemyHitData(new Enemy));
#else
	monster.handle( ExplosionData(40));
	monster.handle( ExplosionData(10));
	monster.handle( EnemyHitData(new Enemy));
	monster.handle( EnemyHitData(new Enemy));
	monster.handle( EnemyHitData(new Enemy));

	std::cout << "\n=== Tank ===" << std::endl; 
	tank.handle(ExplosionData(40));
	tank.handle(ExplosionData(10));
	tank.handle(EnemyHitData(new Enemy));
#endif
	system("PAUSE");
}


