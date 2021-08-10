export const cartReducer=(state={cartItems :[]}, action)=>{
    switch(action.type){
        case 'ADD_TO_CART': return{
            ...state ,
            cartItems:[...state.cartItems, action.payload]
        }
        case 'DELETE_FROM_CART': return{
            loading : false,
            coffees: action.payload
        }
        default: return state
    }
}