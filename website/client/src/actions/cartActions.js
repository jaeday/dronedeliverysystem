import axios from "axios";
export const addToCart=(coffee, quantity, size)=> (dispatch, getState)=>{
    var cartItem = {
        name: coffee.name,
        _id : coffee._id,
        image : coffee.image,
        size : size,
        quantity :quantity,
        prices: coffee.prices,
        price: coffee.prices[0][size]*quantity
    }
        dispatch({type:'ADD_TO_CART', payload: cartItem})

        const cartItems = getState().cartReducer.cartItems
        console.log(cartItem)
        localStorage.setItem('cartItems', JSON.stringify(cartItems))
    
}