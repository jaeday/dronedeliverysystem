import {combineReducers} from 'redux'
import { getAllCoffeesReducer } from './reducers/coffeeReducers'
import {createStore, applyMiddleware} from 'redux'

import thunk from'redux-thunk'

import {composeWithDevTools} from 'redux-devtools-extension'
//import { getAllCoffeesReducer } from './reducers/coffeeReducers'

const finalReducer = combineReducers({
    getAllCoffeesReducer : getAllCoffeesReducer
})

const initialState = {}

const composeEnhancers = composeWithDevTools({})

const store = createStore(finalReducer, initialState, composeEnhancers(applyMiddleware(thunk)))

export default store