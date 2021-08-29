import React from 'react'
import StripeCheckout from 'react-stripe-checkout'
import { useDispatch, useSelector } from 'react-redux';
import { placeOrder } from '../actions/orderActions';
import Loading from '../components/Loading'
import Error from '../components/Error'
import Success from '../components/Success'

export default function Checkout({subtotal}) {
    const orderstate = useSelector((state) => state.placeOrderReducer)
    const {loading, error, success} = orderstate
    const dispatch = useDispatch()
    function tokenHandler(token){
        var dest = localStorage.getItem('location')
        console.log(token);
        dispatch(placeOrder(token, subtotal, dest))

    }
    return (
        <div>
            {loading&&(<loading/>)}
            {error&&(<Error error = 'Something went wrong'/>)}
            {success&&(<Success success = ' Your order was placed successfully'/>)}
            <StripeCheckout amount = {subtotal*100}
            shippingAddress
            token = {tokenHandler}
            stripeKey='pk_test_51JPoR1GdJegYvYHwIRKAK9f2zXMvGwggeC469uPdTIT9X1AnIcFtAp8T3SMiuW8iRDsCpAZyixeBzYOCdTi3ttDq00ZyGvGI73'>
                <button className = 'btn'>
                    Pay Now
                </button>
            </StripeCheckout>
        </div>
    )
}

