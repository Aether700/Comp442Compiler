#pragma once

class IVisitableElement;
class IDNode;

class Visitor
{
public:
    virtual void Visit(IVisitableElement* element) { }
    virtual void Visit(IDNode* element) { }
};


class IVisitableElement
{
public:
    virtual void AcceptVisit(Visitor* visitor) = 0;
};
