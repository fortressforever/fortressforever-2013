### Legend

* __TODO__ Things that need documentation
* *Unconfirmed statements*
* Confirmed statements
* (FF) Fortress Forever specific

# New Entity Implementation Checklist

* __TODO__ Document general entity requirements

## Demos

* __TODO__ Document the requirements for something being replicated in a demo
* __TODO__ Document how ConVars get set when watching someone else's demo and how to include/exclude ConVars from doing that

### SourceTV

* __TODO__ Document the requirements for something being replicated to a SourceTV client or in a SourceTV demo (and what would exclude them)
* *Variables in a _NOBASE DataTable do not get recorded in SourceTV demos*

## Prediction

* __TODO__ Document how to make something properly predicted

## DataTables / Networked Variables

* __TODO__ Document DataTables in how they interact with other people on the server (how to send netwrorked variables to specific clients depending on certain conditions)
* SendProxys are used server-side to determine how the variables in the DataTable are sent (see game/server/sendproxy.cpp)
	* __TODO__ List the available SendProxys (or link to a SendProxy section)

# Example Feature

General description/overview of the feature (optional)

## Usage

* List of guidelines for dealing with the feature
* Information about things like how to implement a new derived class or how to handle a new feature-related entity