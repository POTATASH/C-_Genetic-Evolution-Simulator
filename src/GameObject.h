#pragma once

// Topic: Inheritance & Polymorphism Base Class
class GameObject {
public:
    GameObject(float x, float y) : m_x(x), m_y(y) {}
    virtual ~GameObject() = default; // Topic: Virtual Destructor

    // Topic: Virtual Function (Polymorphism)
    virtual void update() = 0; 

protected:
    float m_x;
    float m_y;
};