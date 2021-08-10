import axios from "axios";
export const addToCart=(coffee, quantity, size)=> (dispatch, getState)=>{
    var cartItem = {
        name: coffee.name,
        _id : coffee._id,
        image : coffee.image,
        size : size,
        quantity : Number(quantity),
        prices: coffee.prices,
        price: coffee.prices[0][size]*quantity
    }
        if(cartItem.quantity>5){
            alert('You cannot add more than 5 quantities.')
        }
        else{
            if(cartItem.quantity<1){
                dispatch({type:'DELETE_FROM_CART', payload:coffee})
            }
            else{
                dispatch({type:'ADD_TO_CART', payload: cartItem})
            }
            
        }
        

        const cartItems = getState().cartReducer.cartItems
        console.log(cartItem)
        localStorage.setItem('cartItems', JSON.stringify(cartItems))
    
}

export const deleteFromCart=(coffee)=>(dispatch, getState)=>{
    dispatch({type:'DELETE_FROM_CART', payload:coffee})
    const cartItems = getState().cartReducer.cartItems
    localStorage.setItem('cartItems', JSON.stringify(cartItems))

}