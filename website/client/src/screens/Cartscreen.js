import React from 'react'
import {useSelector, useDispatch} from 'react-redux'
import { addToCart } from '../actions/cartActions'
import { deleteFromCart } from '../actions/cartActions'

export default function Cartscreen() {

    const cartstate = useSelector(state=>state.cartReducer)
    const cartItems = cartstate.cartItems
    const dispatch = useDispatch()
    var subtotal = cartItems.reduce((x, item) => x+item.price, 0)
    

    return(
        <div className = "row">
            <div className = "col-md-6">
                <h2 style={{fontSize: '30px'}}>My Cart</h2>
                {cartItems.map(item=>{
                    return <div className = "flex-container">
                    <div className='text-left m-1 w-100'>
                        <h1>{item.name} [{item.size}]</h1>
                        {console.log(item.quantity)}
                        <h1>Price : {item.quantity} * {item.prices[0][item.size]} = {item.price}</h1>
                        <h1 style={{display:'inline'}}>Quantity : </h1>
                        <i className="fa fa-plus" aria-hidden="true" onClick={()=>{dispatch(addToCart(item, item.quantity+1, item.size))}}></i>
                        <b>{item.quantity}</b>
                        <i className="fa fa-minus" aria-hidden="true" onClick={()=>{dispatch(addToCart(item, item.quantity-1, item.size))}}></i>
                        <hr/>
                    </div>
                    <div className = 'm-1 w-100'>
                        {console.log(item)}
                        <img src={item.image} style={{height:'80px', width:'80px'}}/>
                    </div>
                    <div className = 'm-1 w-100'>
                        <i className="fa fa-trash mt-5" aria-hidden="true" onClick={()=>{dispatch(deleteFromCart(item, item.quantity-1, item.size))}}></i>
                    </div>
                </div>
                })}

                {cartItems.length >= 6 && <div className = "flex-container">
                    <h1>You are ordering too many coffees at once. As per company policy, we are trying to reduce the delivery time for other students by
                        limiting the amount of coffees each can order. Please take off some of the coffee and try again.
                    </h1>
                    </div>}
                
            </div>
            <div className = "col-md-4 text-right">
                <h2 style = {{fontSize:'45px'}}>SubTotal: ${subtotal}</h2>
                <button className='btn'>CHECK OUT</button>
            </div>


        </div>
    )
}